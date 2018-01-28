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
#include <errno.h>

DIR *__gitpack_get_packdir (const char *path, size_t path_len) {
    char *pack_path = (char *) malloc (path_len + 14);
    if (pack_path == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_packdir: have not enough free memory");
        return NULL;
    }

    strcpy (pack_path, path);
    strcpy (pack_path + path_len, "objects/pack/");
    DIR *ret = opendir (pack_path);
    
    free (pack_path);

    return ret;
}

struct __gitpack *__get_gitpack (const char *path, size_t path_len, const char *idx_name) {
    DBG_LOG (DBG_INFO, "__get_gitpack: malloc struct __gitpack");
    struct __gitpack *ret = (struct __gitpack *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__get_gitpack: have not enough free memory");
        return NULL;
    }

    ret->sign = (char *) malloc (41);
    if (ret->sign == NULL) {
        DBG_LOG (DBG_ERROR, "__get_gitpack: have not enough free memory");
        free (ret);
        return NULL;
    }
    strncpy (ret->sign, idx_name + 5, 40);
    ret->sign[40] = 0;
    DBG_LOG (DBG_INFO, "__get_gitpack: malloc idx_path");
    if (strlen (path) == path_len) {
        DBG_LOG (DBG_INFO, "__get_gitpack: path's string length legal");
    }
    ret->idx_path = (char *) malloc (path_len + 63);
    if (ret->idx_path == NULL) {
        DBG_LOG (DBG_ERROR, "__get_gitpack: have not enough free memory");
        free (ret->sign);
        free (ret);
        return NULL;
    }
    strcpy (ret->idx_path, path);
    strcpy (ret->idx_path + path_len, "objects/pack/pack-");
    strcpy (ret->idx_path + path_len + 18, ret->sign);
    strcpy (ret->idx_path + path_len + 58, ".idx");

    ret->pack_path = (char *) malloc (path_len + 64);
    if (path == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_packfile_path_get: have not enough free memory");
        free (ret->sign);
        free (ret->idx_path);
        free (ret);
        return NULL;
    }
    strcpy (ret->pack_path, path);
    strcpy (ret->pack_path + path_len, "objects/pack/pack-");
    strncpy (ret->pack_path + path_len + 18, ret->sign, 40);
    strcpy (ret->pack_path + path_len + 58, ".pack");

    ret->count = 0;
    ret->rd_tree = NULL;
    ret->prev = ret->next = NULL;
    return ret;
}

void __gitpack_dtor (struct __gitpack *pack) {
    if (pack == NULL) return;
    if (pack->sign != NULL) free (pack->sign);
    if (pack->idx_path != NULL) free (pack->idx_path);
    if (pack->pack_path != NULL) free (pack->pack_path);
    if (pack->rd_tree != NULL) rdt_dtor (pack->rd_tree);
    if (pack->indexes != NULL) {
        int i;
        for (i = 0; i < pack->count; i++) free (pack->indexes[i].sign);
        free (pack->indexes);
    }
    free (pack);
}

void __gitpack_collection_dtor (struct __gitpack_collection *obj) {
    if (obj == NULL) return;

    while (obj->head != NULL) {
        struct __gitpack *next = obj->head->next;
        __gitpack_dtor (obj->head);
        obj->head = next;
    }

    free (obj);
}

struct __opend_mmap_file {
    int fd;
    unsigned char *val;
    size_t len;
};

void __gitpack_idxfile_close (struct __opend_mmap_file *mmaped) {
    if (mmaped == NULL) return;
    munmap (mmaped->val, mmaped->len);
    if (close (mmaped->fd) == -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_idx_file_close: ");
        DBG_LOG (DBG_ERROR, strerror (errno));
    }
    free (mmaped);
}

struct __opend_mmap_file *__gitpack_idxfile_open (struct __gitpack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_idxfile_open: pack is null");
        return NULL;
    }
    struct __opend_mmap_file *ret = (struct __opend_mmap_file *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_idxfile_open: have not enough free memory");
        return NULL;
    }

    ret->fd = open (pack->idx_path, O_RDONLY);
    if (ret->fd == -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_idxfile_open: idx file cannot opend");
        DBG_LOG (DBG_ERROR, strerror (errno));
        free (ret);
        return NULL;
    }
    printf ("%d\n", ret->fd);
    struct stat idx_st;
    if (fstat (ret->fd, &idx_st) != 0) {
        DBG_LOG (DBG_ERROR, "__gitpack_idxfile_open: idx file cannot get st");
        close (ret->fd);
        free (ret);
        return NULL;
    }
    ret->len = idx_st.st_size;
    ret->val = mmap (NULL, idx_st.st_size, PROT_READ, MAP_SHARED, ret->fd, 0);
    if ((void *) ret->val == (void *) -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_idxfile_open: idx file cannot mmap");
        close (ret->fd);
        free (ret);
        return NULL;
    }

    return ret;
}

int __gitpack_get_count (struct __opend_mmap_file *mmaped) {
    if (mmaped == NULL) return 0;
    unsigned int *p = (unsigned int *) (mmaped->val + 8);
    int count = 0;
    int i;
    for (i = 0; i < 256; i++) {
        int n = ntohl (p[i]);
        if (n > count) count = n;
    }

    return count;
}

#define __GITPACK_NTH_SIGN(val, n) ((val) + 8 + 1024 + 20 * (n))
#define __GITPACK_NTH_OFF(val, nr, n) ntohl (*((int *) ((val) + 8 + 1024 + 24 * (nr) + 4 * (n))))

void __gitpack_quicksort_indexes (unsigned char *val, struct __gitpack *pack, struct __gitpack_index *indexes, int start, int end) {
    if (start >= end) return;

    int i = start;
    int j = end;
    struct __gitpack_index k = indexes[start];

    while (i < j) {
        while (i < j && k.off < indexes[j].off) j--;
        indexes[i] = indexes[j];
        while (i < j && indexes[i].off < k.off) i++;
        indexes[j] = indexes[i];
    }
    indexes[i] = k;

    __gitpack_quicksort_indexes (val, pack, indexes, start, i - 1);
    __gitpack_quicksort_indexes (val, pack, indexes, i + 1, end);
}

void * __sign_dup (void *key) {
    void *ret = (void *) malloc (20);
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__sign_dup: have not enough free memory");
        return NULL;
    }
    
    memcpy (ret, key, 20);

    return ret;
}

struct __gitpack_index *__gitpack_get_sortedindexes (unsigned char *val, struct __gitpack *pack, size_t packsize) {
    // this malloc may occur error.
    struct __gitpack_index *ret = (struct __gitpack_index *) malloc (sizeof (*ret) * pack->count);
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_sortedindexes: have not enough free memory");
        return NULL;
    }
    int i = 0;
    for (i = 0; i < pack->count; i++) {
        struct __gitpack_index index = { i, __GITPACK_NTH_OFF(val, pack->count, i), 0, NULL };
        ret[i] = index;
        ret[i].sign = __sign_dup(__GITPACK_NTH_SIGN (val, i));
    }

    __gitpack_quicksort_indexes (val, pack, ret, 0, pack->count - 1);

    ret[pack->count - 1].len = packsize - 20 - ret[pack->count - 1].off;
    for (i = 0; i < pack->count - 1; i++) {
        ret[i].len = ret[i + 1].off - ret[i].off;
    }
    return ret;
}

struct rdt *__gitpack_build_redblacktree (struct __gitpack_index *indexes, size_t count) {
    if (indexes == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_build_redblacktree: indexes is null");
        return NULL;
    }
    struct rdt *ret = rdt_build ();
    int i = 0;
    for (i = 0; i < count; i++) {
        rdt_insert (ret, indexes[i].sign, indexes[i].off, indexes[i].len);
    }

    return ret;
}

size_t __gitpack_get_size (struct __gitpack *pack) {
    struct stat st;
    if (stat (pack->pack_path, &st) != 0) {
        return 0;
    }
    return st.st_size;
}

struct __gitpack_collection *__gitpack_get_collection (struct gitrepo *repo) {
    if (repo == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_collection: repo is null");
        return NULL;
    }
    struct __gitpack_collection *ret = (struct __gitpack_collection *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_collection: have not enough free memory");
        return NULL;
    }
    ret->head = ret->tail = NULL;
    if (repo->path == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_collection: gitrepo's path is null");
        return NULL;
    }
    size_t path_len = strlen (repo->path);

    DIR *dir = __gitpack_get_packdir (repo->path, path_len);
    if (dir == NULL) {
        free (ret);
        return NULL;
    }

    struct dirent *ent;
    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_REG) {
            if (strcmp (ent->d_name + 45, ".idx") != 0) continue;

            struct __gitpack *pack = __get_gitpack (repo->path, path_len, ent->d_name);
            if (pack == NULL) {
                __gitpack_collection_dtor (ret);
                closedir (dir);
                return NULL;
            }

            struct __opend_mmap_file *idx_mmaped = __gitpack_idxfile_open (pack);
            if (idx_mmaped == NULL) {
                __gitpack_collection_dtor (ret);
                closedir (dir);
                return NULL;
            }
            // get count
            pack->count = __gitpack_get_count (idx_mmaped);
            // build linear indexes
            pack->indexes = __gitpack_get_sortedindexes (idx_mmaped->val, pack, __gitpack_get_size (pack));
            // build red black tree by indexes
            pack->rd_tree = __gitpack_build_redblacktree (pack->indexes, pack->count);

            __gitpack_idxfile_close (idx_mmaped);

            if (ret->head == NULL) ret->head = pack;
            if (ret->tail != NULL) ret->tail->next = pack;
            ret->tail = pack;
        }
    }
    closedir (dir);

    return ret;
}