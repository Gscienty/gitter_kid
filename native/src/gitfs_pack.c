#include "gitfs.h"
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <limits.h>

struct git_packes *git_packes_get (struct git_repo *repo) {
    if (repo == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: repo is null");
        return NULL;
    }

    int repo_path_length = strlen (repo->path);
    char *pack_path = (char *) malloc (sizeof (*pack_path) * (repo_path_length + 14));
    if (pack_path == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
        return NULL;
    }
    strcpy (pack_path, repo->path);
    strcpy (pack_path + repo_path_length, "objects/pack/");
    pack_path[repo_path_length + 13] = 0;

    struct git_packes *result = (struct git_packes *) malloc (sizeof (*result));
    if (result == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
        free (pack_path);
        return NULL;
    }
    result->head = result->tail = result->cursor = NULL;
    struct dirent *ent;
    DIR *dir = opendir (pack_path);
    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_REG) {
            if (strcmp (ent->d_name + 45, ".idx") != 0) {
                continue;
            }
            struct git_pack *pack = (struct git_pack *) malloc (sizeof (*pack));
            if (pack == NULL) {
                DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
                git_packes_dispose (result);
                closedir (dir);
                free (pack_path);
                return NULL;
            }
            pack->idx_path = (char *) malloc (sizeof (char) * (repo_path_length + 63));
            if (pack->idx_path == NULL) {
                DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
                git_packes_dispose (result);
                closedir (dir);
                free (pack);
                free (pack_path);
                return NULL;
            }
            strcpy (pack->idx_path, pack_path);
            strcpy (pack->idx_path + repo_path_length + 13, ent->d_name);
            pack->idx_path[repo_path_length + 62] = 0;
            pack->idx_map = NULL;
            pack->idx_fd = 0;
            pack->idx_size = 0;
            pack->count = 0;
            pack->rdtree = NULL;

            pack->next = NULL;
            pack->prev = result->tail;
            if (result->head == NULL) {
                result->head = pack;
            }
            if (result->tail != NULL) {
                result->tail->next = pack;
            }
            result->tail = pack;
        }
    }
    closedir (dir);
    return result;
}

void git_packes_dispose (struct git_packes *packes) {
    if (packes == NULL) {
        return ;
    }
    while (packes->head) {
        struct git_pack *next_pack = packes->head->next;
        free (packes->head->idx_path);
        if (packes->head->idx_map != NULL) {
            munmap (packes->head->idx_map, packes->head->idx_size);
        }
        if (packes->head->idx_fd != 0) {
            close (packes->head->idx_fd);
        }
        free (packes->head);
        packes->head = next_pack;
    }
    free (packes);

    // TODO: need dispost other fields
}

void git_packes_reset (struct git_packes *packes) {
    if (packes == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_reset: packes is null");
        return ;
    }
    packes->cursor = packes->head;
}

struct git_pack *git_packes_get_current (struct git_packes *packes) {
    if (packes == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get_current: packes is null");
        return NULL;
    }
    return packes->cursor;
}

int git_packes_move_next (struct git_packes *packes) {
    if (packes == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_move_next: packes is null");
        return -1;
    }
    if (packes->cursor == NULL) {
        DBG_LOG (DBG_INFO, "git_packes_move_next: packes's cursor is null");
        return -1;
    }

    return (packes->cursor = packes->cursor->next) == NULL ? -1 : 0;
}

int git_pack_is_open (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "git_pack_is_open: pack is null");
        return 0;
    }
    return pack->idx_map != NULL;
}

int git_pack_open (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "git_pack_open: pack is null");
        return -1;
    }

    pack->idx_fd = open (pack->idx_path, O_RDONLY);
    if (pack->idx_fd == -1) {
        DBG_LOG (DBG_ERROR, "git_pack_open: idx file cannnot opened");
        return -2;
    }
    struct stat st;
    int retval = fstat (pack->idx_fd, &st);
    if (retval != 0) {
        DBG_LOG (DBG_ERROR, "git_pack_open: idx file cannot got stat");
        return -3;
    }
    pack->idx_size = st.st_size;
    pack->idx_map = mmap (NULL, st.st_size, PROT_READ, MAP_SHARED, pack->idx_fd, 0);
    if (pack->idx_map == (void *) -1) {
        close (pack->idx_fd);
        pack->idx_fd = 0;
        pack->idx_map = NULL;
        DBG_LOG (DBG_ERROR, "git_pack_open: idx file cannot mmap");
        return -4;
    }
    pack->count = __git_pack_count (pack);
    if (pack->count == 0) {
        close (pack->idx_fd);
        pack->idx_fd = 0;
        pack->idx_map = NULL;
        DBG_LOG (DBG_ERROR, "git_pack_open: idx file cannot got count");
        return -5;
    }

    __git_pack_build_rdtree (pack);
    return 0;
}

int __git_pack_count (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_count: pack is null");
        return 0;
    }
    if (pack->idx_map == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_count: have not open idx file");
        return 0;
    }

    unsigned int *p = pack->idx_map + 8;
    int i;
    int count = 0;
    for (i = 0; i < 256; i++) {
        int n = ntohl(p[i]);
        if (n > count) {
            count = n;
        }
    }

    return count;
}

void *__git_pack_get_nth_signture (struct git_pack *pack, int n) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_nth_signture: pack is null");
        return NULL;
    }
    if (pack->idx_map == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_nth_signture: pack need open idx file");
        return NULL;
    }
    return pack->idx_map + 8 + 1024 + 20 * n;
}

int __git_pack_get_nth_offset (struct git_pack *pack, int n) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_nth_signture: pack is null");
        return 0;
    }
    if (pack->idx_map == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_nth_signture: pack need open idx file");
        return 0;
    }

    unsigned int *retval = (int *) (pack->idx_map + 8 + 1024 + 24 * pack->count + 4 * n);
    return ntohl (*retval);
}

void __git_pack_build_rdtree_qsort_indexes (struct git_pack *pack, int *indexes, int start, int end) {
    if (start >= end) {
        return;
    }

    int i = start;
    int j = end;
    int k = indexes[start];
    while (i < j) {
        while (i < j && __git_pack_get_nth_offset (pack, k) < __git_pack_get_nth_offset (pack, indexes[j])) {
            j--;
        }
        indexes[i] = indexes[j];
        while (i < j && __git_pack_get_nth_offset (pack, indexes[i]) < __git_pack_get_nth_offset (pack, k)) {
            i++;
        }
        indexes[j] = indexes[i];
    }
    indexes[i] = k;

    __git_pack_build_rdtree_qsort_indexes (pack, indexes, start, i - 1);
    __git_pack_build_rdtree_qsort_indexes (pack, indexes, i + 1, end);
}

void __git_pack_build_rdtree (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_build_rdtree: pack is null");
        return;
    }
    if (pack->idx_map == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_build_rdtree: pack need open idx file");
        return;
    }
    if (pack->rdtree != NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_build_rdtree: already exist red black tree");
        return;
    }

    int *indexes = (int *) malloc (sizeof (*indexes) * pack->count);
    int i;
    for (i = 0; i < pack->count; i++) {
        indexes[i] = i;
    }
    __git_pack_build_rdtree_qsort_indexes (pack, indexes, 0, pack->count - 1);

    int base_len = strlen (pack->idx_path);
    char *packfile_path = (char *) malloc (sizeof (char) * (base_len + 2));

    strncpy (packfile_path, pack->idx_path, base_len - 3);
    strcpy (packfile_path + base_len - 3, "pack");

    struct stat st;

    if (stat (packfile_path, &st) != 0) {
        DBG_LOG (DBG_ERROR, "__git_pack_build_rdtree: cannot get .pack file stat");
        free (packfile_path);
        free (indexes);
        return;
    }
    free (packfile_path);

    pack->rdtree = rdt_build ();
    for (i = 0; i < pack->count; i++) {
        rdt_insert (
            pack->rdtree,
            __git_pack_get_nth_signture (pack, indexes[i]),
            __git_pack_get_nth_offset (pack, indexes[i]),
            i + 1 == pack->count
                ? st.st_size - __git_pack_get_nth_offset (pack, indexes[i])
                : __git_pack_get_nth_offset (pack, indexes[i + 1]) - __git_pack_get_nth_offset (pack, indexes[i])
        );
    }

    free (indexes);
}

int __git_get_packfd (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_get_packfd: pack is null");
        return -1;
    }

    // get pack file;
    int basepath_length = strlen (pack->idx_path);
    char *packfile_path = (char *) malloc (sizeof (char) * (basepath_length + 2));
    strncpy (packfile_path, pack->idx_path, basepath_length - 3);
    strcpy (packfile_path + basepath_length - 3, "pack");

    int packfd = open (packfile_path, O_RDONLY);
    free (packfile_path);
    return packfd;
}

struct __git_packitem {
    void *mmaped_base;
    int inner_offset;
    int _n;
    int mmaped_len;

    unsigned char item_type;
    int origin_len;

    int packfd;
};

struct __git_packitem *__git_packitem_mmap (struct git_pack *pack, int packfile_offset, int pack_len) {
    struct __git_packitem *result = (struct __git_packitem *) malloc (sizeof (*result));
    if (result == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_mmap: have not enough free memory");
        return NULL;
    }

    // get memory page
    const int page_size = sysconf (_SC_PAGESIZE);
    // calc nth memory page count
    int nth_page = packfile_offset / page_size;
    // inner page offset
    int page_inner_offset = packfile_offset - (nth_page * page_size);
    // calc physical len
    result->mmaped_len = pack_len + page_inner_offset;

    // open pack file
    result->packfd = __git_get_packfd (pack);
    if (result->packfd == -1) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_obj: cannot open pack file");
        return NULL;
    }

    // mmap file's sub-content
    result->mmaped_base = mmap (NULL, result->mmaped_len, PROT_READ, MAP_SHARED, result->packfd, page_size * nth_page);
    if (result->mmaped_base == (void *) -1) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_obj: map failure");
        free (result);
        return NULL;
    }
    

    // calc item type & inflated content length
    result->item_type = ((*(unsigned char *) (result->mmaped_base + page_inner_offset)) & 0x70);
    result->origin_len = ((*(unsigned char *) (result->mmaped_base + page_inner_offset)) & 0x0F);
    result->inner_offset = page_inner_offset;
    while (((*(unsigned char *) (result->mmaped_base + result->inner_offset)) & 0x80) != 0) {
        result->inner_offset++;
        result->origin_len = (result->origin_len << 7)
            | ((*(unsigned char *) (result->mmaped_base + result->inner_offset)) & 0x7F);
    }
    result->inner_offset++;
    result->_n = result->inner_offset - page_inner_offset;

    return result;
}

void __git_packitem_dispose (struct __git_packitem *mmaped) {
    if (mmaped == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_dispose: mmaped is null");
        return;
    }
    if (mmaped->mmaped_base != 0) {
        munmap (mmaped->mmaped_base, mmaped->mmaped_len);
    }
    if (mmaped->packfd != 0) {
        close (mmaped->packfd);
    }
    free (mmaped);
}

struct __obj_file_ret *__git_packitem_inflate (struct __git_packitem *packitem) {
    struct __obj_file_ret *deflated_obj = (struct __obj_file_ret *) malloc (sizeof (*deflated_obj));
    if (deflated_obj == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_obj: have not enough free memory");
        return NULL;
    }
    deflated_obj->buf = packitem->mmaped_base + packitem->inner_offset;
    deflated_obj->len = packitem->mmaped_len - packitem->inner_offset;
    return __inflate (deflated_obj, packitem->origin_len);   
}

struct git_obj_ref_delta *__git_obj_transfer_ref_delta (struct git_obj *obj, int deflated_len) {
    struct git_obj_ref_delta *ret = (struct git_obj_ref_delta *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__git_obj_transfer_ref_delta: have not enough free memory");
        return NULL;
    }
    ret->sign = (char *) malloc (sizeof (char) * 20);
    if (ret->sign == NULL) {
        DBG_LOG (DBG_ERROR, "__git_obj_transfer_ref_delta: have not enough free memory");
        free (ret);
        return NULL;
    }
    memcpy (ret->sign, obj->body, 20);
    
    struct __obj_file_ret *deflated_buffer = (struct __obj_file_ret *) malloc (sizeof (*deflated_buffer));
    deflated_buffer->buf = obj->body + 20;
    deflated_buffer->len = obj->size - 20;
    struct __obj_file_ret *inflated_buffer = __inflate (deflated_buffer, deflated_len);
    free (deflated_buffer);

    ret->content = inflated_buffer->buf;
    ret->len = inflated_buffer->len;

    free (inflated_buffer);

    return ret;
}

struct git_obj_ofs_delta *__git_obj_transfer_ofs_delta (struct git_obj *obj, int n, int deflated_len) {
    struct git_obj_ofs_delta *ret = (struct git_obj_ofs_delta *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__git_obj_transfer_ofs_delta: have not enough free memory");
        return NULL;
    }
    ret->offset = 0;
    int i;
    for (i = 0; i < n; i++) {
        ret->offset = (ret->offset << 7) | (*(unsigned char *) (obj->body + i) & 0x7F);
    }

    struct __obj_file_ret *deflated_buffer = (struct __obj_file_ret *) malloc (sizeof (*deflated_buffer));
    deflated_buffer->buf = obj->body + n;
    deflated_buffer->len = obj->size - n;
    struct __obj_file_ret *inflated_buffer = __inflate (deflated_buffer, deflated_len);
    free (deflated_buffer);

    ret->content = inflated_buffer->buf;
    ret->len = inflated_buffer->len;

    free (inflated_buffer);
    
    return ret;
}

struct __git_packitem *__git_pack_get_packitem (struct git_pack *pack, const char *signture) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_obj: pack is null");
        return NULL;
    }
    if (signture == NULL || strlen (signture) != 40) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_obj: signture illegal");
        return NULL;
    }

    // find idx's node what contains pack item's offset & len by red black tree
    struct rdt_node *node = rdt_find (pack->rdtree, signture);
    if (node == NULL) {
        DBG_LOG (DBG_INFO, "__git_pack_get_obj: have not finded entry");
        return NULL;
    }

    // get deflated pack item
    struct __git_packitem *packitem = __git_packitem_mmap (pack, node->offset, node->len);
    if (packitem == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_obj: cannot get pack item");
        return NULL;
    }

    return packitem;
}

struct git_obj *__git_pack_get_obj (struct git_pack *pack, const char *signture) {
    struct git_obj *ret = (struct git_obj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_obj: have not enough free memory");
        return NULL;
    }
    ret->path = NULL;
    ret->sign = strdup (signture);
    
    struct __git_packitem *packitem = __git_pack_get_packitem (pack, signture);
    if (packitem == NULL) {
        // not find item
        free (ret->sign);
        free (ret);
        return NULL;
    }

    // get object type
    switch (packitem->item_type) {
        case 0x10: ret->type = GIT_OBJ_TYPE_COMMIT; break;
        case 0x20: ret->type = GIT_OBJ_TYPE_TREE; break;
        case 0x30: ret->type = GIT_OBJ_TYPE_BLOB; break;
        case 0x40: ret->type = GIT_OBJ_TYPE_TAG; break;
        case 0x60: ret->type = GIT_OBJ_TYPE_OFS_DELTA; break;
        case 0x70: ret->type = GIT_OBJ_TYPE_REF_DELTA; break;
        default: ret->type = GIT_OBJ_TYPE_UNKNOW; break;
    }

    if (ret->type == GIT_OBJ_TYPE_REF_DELTA) {
        ret->size = packitem->mmaped_len - packitem->inner_offset;
        ret->buf = (void *) malloc (ret->size);
        memcpy (ret->buf, packitem->mmaped_base + packitem->inner_offset, ret->size);
        ret->body = ret->buf;
        ret->ptr = __git_obj_transfer_ref_delta (ret, packitem->origin_len);

        __git_packitem_dispose (packitem);

        printf ("%s\n", (char *) ((struct git_obj_ref_delta *) ret->ptr)->content);
    }
    else if (ret->type == GIT_OBJ_TYPE_OFS_DELTA) {
        ret->size = packitem->mmaped_len - packitem->inner_offset;
        ret->buf = (void *) malloc (ret->size);
        memcpy (ret->buf, packitem->mmaped_base + packitem->inner_offset, ret->size);
        ret->body = ret->buf;
        ret->ptr = __git_obj_transfer_ofs_delta (ret, packitem->_n, packitem->origin_len);

        __git_packitem_dispose (packitem);

        printf ("%d\n", packitem->origin_len);
        printf ("%d\n", ret->size);
        printf ("%d\n", (unsigned int) ((struct git_obj_ofs_delta *) ret->ptr)->offset);
        printf ("%s\n", (char *) (((struct git_obj_ofs_delta *) ret->ptr)->content));
    }
    else {
        // get object length
        ret->size = packitem->origin_len;
        // inflate pack item
        struct __obj_file_ret *inflated_packitem = __git_packitem_inflate (packitem);

        __git_packitem_dispose (packitem);
        if (inflated_packitem == NULL) {
            DBG_LOG (DBG_ERROR, "__git_pack_get_obj: cannot infalte packitem");
            return NULL;
        }
        ret->buf = inflated_packitem->buf;
        ret->body = ret->buf;
        ret->ptr = NULL;
    }
    printf ("%d\t%d\n", ret->size, ret->type);
    return ret;
}