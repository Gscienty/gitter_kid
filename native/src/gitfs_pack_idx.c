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

DIR *__gitpack_packdir_get (const char *path, size_t path_len) {
    char *pack_path = (char *) malloc (sizeof (char) * (path_len + 14));
    if (pack_path) {
        DBG_LOG (DBG_ERROR, "__gitpack_packdir_get: have not enough free memory");
        return NULL;
    }

    strcpy (pack_path, path);
    strcpy (pack_path + path_len, "objects/pack/");
    
    DIR *ret = opendir (pack_path);

    free (pack_path);

    return ret;
}

struct __gitpack *__gitpack_pack_get (const char *idx_name) {
    struct __gitpack *ret = (struct __gitpack *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_pack_get: have not enough free memory");
        return NULL;
    }

    ret->sign = (char *) malloc (sizeof (char) * 21);
    if (ret->sign == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_pack_get: have not enough free memory");
        free (ret);
        return NULL;
    }
    strncpy (ret->sign, idx_name + 5, 20);
    ret->count = 0;
    ret->rd_tree = NULL;
    ret->prev = ret->next = NULL;

    return ret;
}

void __gitpack_collection_dispose (struct __gitpack_collection *obj) {
    if (obj == NULL) {
        return ;
    }

    // TODO: dispose
}

struct __opend_mmap_file {
    int fd;
    unsigned char *val;
    size_t len;
};

void __gitpack_idxfile_close (struct __opend_mmap_file *mmaped) {
    if (mmaped == NULL) return;
    munmap (mmaped->val, mmaped->len);
    close (mmaped->fd);
}

struct __opend_mmap_file *__gitpack_idxfile_open (const char *path, size_t path_len, struct __gitpack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_count_get: pack is null");
        return NULL;
    }
    struct __opend_mmap_file *ret = (struct __opend_mmap_file *) malloc (sizeof (ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_count_get: have not enough free memory");
        return NULL;
    }

    char *idx_path = (char *) malloc (sizeof (char) * (path_len + 43));
    if (idx_path == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_count_get: have not enough free memory");
        free (ret);
        return NULL;
    }
    strcpy (idx_path, path);
    strcpy (idx_path + path_len, "objects/pack/pack-");
    strcpy (idx_path + path_len + 18, pack->sign);
    strcpy (idx_path + path_len + 38, ".idx");

    ret->fd = open (idx_path, O_RDONLY);
    free (idx_path);
    if (ret->fd == -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_count_get: idx file cannot opend");
        free (ret);
        return NULL;
    }
    struct stat idx_st;
    if (fstat (ret->fd, &st) != 0) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_count_get: idx file cannot get st");
        close (ret->fd);
        free (ret);
        return NULL;
    }
    ret->len = st.st_size;
    ret->val = mmap (NULL, st.st_size, PROT_READ, MAP_SHARED, ret->fd, 0);
    if ((void *) ret->val == (void *) -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_count_get: idx file cannot mmap");
        close (ret->fd);
        free (ret);
        return NULL;
    }

    return ret;
}

struct __gitpack_collection *__gitpack_collection_get (struct git_repo *repo) {
    if (repo == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_collection_get: repo is null");
        return NULL;
    }

    struct __gitpack_collection *ret = (struct __gitpack_collection *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_collection_get: have not enough free memory");
        return NULL;
    }
    ret->head = ret->tail = ret->cursor = NULL;

    int path_len = strlen (repo->path);
    DIR *dir = __gitpack_packdir_get (repo->path, path_len);
    if (dir == NULL) {
        free (ret);
        return NULL;
    }

    struct dirent *ent;
    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_REG) {
            if (strcmp (ent->d_name + 45, ".idx") != 0) continue;

            struct __gitpack *pack = __gitpack_collection_get (ent->d_name);
            if (pack == NULL) {
                __gitpack_collection_dispose (ret);
                close (dir);
                return NULL;
            }
            struct __opend_mmap_file *mmaped = (repo->path, path_len, pack);
            if (mmaped == NULL) {
                __gitpack_collection_dispose (ret);
                close (dir);
                return NULL;
            }

            // get count
            pack->count = __gitpack_item_count_get (mmaped, pack);
            // build red black tree
            pack->rd_tree = __gitpack_rdt_build (mmaped, pack);
            
            __gitpack_idxfile_close (mmaped);

            if (result->head == NULL) result->head = pack;
            if (result->tail != NULL) result->tail->next = pack;
            result->tail = pack;
        }
    }
    closedir (dir);

    return ret;
}

int __gitpack_item_count_get (struct __opend_mmap_file *mmaped, struct __gitpack *pack) {
    if (mmaped == NULL) return 0;
    unsigned int *p = mmaped->val + 8;
    int count = 0;
    int i;
    for (i = 0; i < 256; i++) {
        int n = ntohl (p[i]);
        if (n > count) count = n;
    }

    return count;
}

#define __GITPACK_NTH_SIGN (val, n) ((val) + 8 + 1024 + 20 * (n))
#define __GITPACK_NTH_OFF (val, nr, n) ntohl (*(int *) ((val) + 8 + 1024 + 24 * (nr) + 4 * (n)))

void __gitpack_quicksort_indexes (unsigned char *val, struct __gitpack *pack, int *indexes, int start, int end) {
    if (start >= end) return;

    int i = start;
    int j = end;
    int k = indexes[start];

    while (i < j) {
        while (i < j && __GITPACK_NTH_OFF (val, pack->count, k) < __GITPACK_NTH_OFF (val, pack->count, indexes[j])) j--;
        indexes[i] = indexes[j];
        while (i < j && __GITPACK_NTH_OFF (val, pack->count, indexes[i]) < __GITPACK_NTH_OFF (val, pack->count, k)) i++;
        indexes[j] = indexes[i];
    }
    indexes[i] = k;

    __gitpack_quicksort_indexes (val, pack, start, i - 1);
    __gitpack_quicksort_indexes (val, pack, i + 1, end);
}

int *__gitpack_sortedindexes_get (unsigned char *val, struct __gitpack *pack) {
    int *ret = (int *) malloc (sizeof (*ret) * pack->count);
    int i = 0;
    for (i = 0; i < pack->count; i++) ret[i] = i;

    __gitpack_quicksort_indexes (val, ret, 0, pack->count - 1);

    return ret;
}

struct rdt *__gitpack_rdt_build (struct __opend_mmap_file *mmaped, struct __gitpack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_count_get: pack is null");
        return NULL;
    }
    if (mmaped == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_count_get: mmaped is null");
        return NULL;
    }
    struct rdt *ret = rdt_build ();

    int *indexes = __gitpack_sortedindexes_get (mmaped->val, pack);
    for (i = 0; i < pack->count; i++) {
        rdt_insert (
            ret,
            __GITPACK_NTH_SIGN (mmaped->val, indexes[i]),
            __GITPACK_NTH_OFF (mmaped->val, pack->count, indexes[i]),
            i + 1 == pack->count
                ? mmaped->len - 20 - __GITPACK_NTH_OFF (mmaped->val, pack->count, indexes[i])
                : __GITPACK_NTH_OFF (mmaped->val, pack->count, indexes[i + 1]) - __GITPACK_NTH_OFF (mmaped->val, pack->count, indexes[i])
        );
    }
    free (indexes);

    return ret;
}