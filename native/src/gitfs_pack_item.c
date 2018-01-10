#include "gitfs.h"
#include <string.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <limits.h>

// find node (segment's offset & len) by red black tree (idx)
struct rdt_node *__gitpack_rdtnode_find (pack, sign, find_func)
    struct __gitpack *pack;
    const void *sign;
    struct rdt_node *(*find_func) (const struct rdt *tree, const void *key);
{
    return find_func (pack->rd_tree, sign);
}

struct __gitpack_item_findret {
    struct __gitpack *pack;
    struct rdt_node *node;
};

// find node & pack from pack collection
struct __gitpack_item_findret *__gitpack_collection_rdtnode_find (collection, sign, find_func)
    struct __gitpack_collection *collection;
    const void *sign;
    struct rdt_node *(*find_func) (const struct rdt *tree, const void *key);
{
    struct __gitpack *cursor = collection->head;
    while (cursor != NULL) {
        struct rdt_node *findnode = __gitpack_rdtnode_find (cursor, sign, find_func);

        if (findnode != NULL) {
            struct __gitpack_item_findret *ret = (struct __gitpack_item_findret *) malloc (sizeof (*ret));
            if (ret == NULL) {
                DBG_LOG (DBG_ERROR, "__gitpack_collection_rdtnode_find: have not enough free memory");
                return NULL;
            }
            
            ret->node = findnode;
            ret->pack = cursor;

            return ret;
        }

        cursor = cursor->next;
    }
    return NULL;
}


// build .pack file's path
char *__gitpack_packfile_path_get (const char *repo_path, size_t repo_path_len, const char *sign) {
    size_t len = repo_path_len + 64;
    char *ret = (char *) malloc (sizeof (char) * len);
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_packfile_path_get: have not enough free memory");
        return NULL;
    }
    strcpy (ret, repo_path);
    strcpy (ret + repo_path_len, "objects/pack/pack-");
    strncpy (ret + repo_path_len + 18, sign, 40);
    strcpy (ret + repo_path_len + 58, ".pack");

    return ret;
}

struct __packfile_mmap {
    int fd;

    void *base;
    size_t off;
    size_t len;
};

// map .pack file's segment which the off & len descript to main memory.
struct __packfile_mmap *__gitpack_mmap (struct __gitpack_collection *collection, struct __gitpack_item_findret *finded_pack) {
    char *path = __gitpack_packfile_path_get (collection->repo_path, collection->repo_path_len, finded_pack->pack->sign);
    if (path == NULL) return NULL;

    struct __packfile_mmap *ret = (struct __packfile_mmap *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_mmap: have not enough free memory");
        free (path);
        return NULL;
    }
    ret->fd = open (path, O_RDONLY);
    free (path);
    if (ret->fd == -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_mmap: cannot open fd");
        free (ret);
        return NULL;
    }

    const size_t MEMORY_PAGESIZE = sysconf (_SC_PAGESIZE);
    int nth_page_start = finded_pack->node->off / MEMORY_PAGESIZE;

    ret->off = finded_pack->node->off % MEMORY_PAGESIZE;
    ret->len = finded_pack->node->len;

    ret->base = mmap (NULL, ret->len + ret->off, PROT_READ, MAP_SHARED, ret->fd, nth_page_start * MEMORY_PAGESIZE);
    if (ret->base == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_mmap: pack file cannot mapped to memory");
        close (ret->fd);
        free (ret);
        return NULL;
    }
    return ret;
}

// dispose __packfile_mmap
void __gitpack_mmap_dispose (struct __packfile_mmap *mmaped) {
    if (mmaped == NULL) return;
    const size_t MEMORY_PAGESIZE = sysconf (_SC_PAGESIZE);

    if (mmaped->base != NULL) munmap (mmaped->base, mmaped->len + mmaped->off);
    if (mmaped->fd != -1) close (mmaped->fd);
    free (mmaped);
}

struct git_obj *__gitpack_wrap_obj (void *base, size_t len) {
    struct git_obj *ret = (struct git_obj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_wrap_obj: have not enough free memory");
        return NULL;
    }

    ret->sign = (char *) malloc (sizeof (char) * 41);
    if (ret->sign == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_wrap_obj: have not enough free memory");
        free (ret);
        return NULL;
    }
    ret->sign[40] = 0;
    ret->buf = base;
    ret->path = NULL;
    ret->type = GIT_OBJ_TYPE_UNKNOW;
    ret->size = len;
    ret->body = base;
    ret->ptr = NULL;

    return ret;
}

#define HEX(v) ((v) < 10 ? '0' + (v) : (v) - 10 + 'a')

// get git object by pack
struct git_obj *__gitpack_obj_get (collection, finded_pack)
    struct __gitpack_collection *collection;
    struct __gitpack_item_findret *finded_pack;
{
    struct __packfile_mmap *mmaped = __gitpack_mmap (collection, finded_pack);
    if (mmaped == NULL) return NULL;

    unsigned char *ptr = mmaped->base + mmaped->off;

    unsigned char packitem_type = *ptr & 0x70;
    size_t origin_len = *ptr & 0x0F;
    size_t deflate_len = mmaped->len - 1;
    while (*ptr & 0x80) {
        ptr++;
        origin_len <<= 7;
        origin_len |= *ptr & 0x7F;
        deflate_len--;
    }
    ptr++;

    struct __deflate_param deflated_obj = { ptr, deflate_len };
    struct __deflate_param *inflated_obj = __inflate (&deflated_obj, origin_len);

    struct git_obj *ret = __gitpack_wrap_obj (inflated_obj->buf, inflated_obj->len);
    if (ret == NULL) {
        __gitpack_mmap_dispose (mmaped);
        free (inflated_obj->buf);
        free (inflated_obj);

        return NULL;
    }

    int i;
    for (i = 0; i < 20; i++) {
        ret->sign[i << 1] = HEX ((((unsigned char *) finded_pack->node->key)[i] & 0xF0) >> 4);
        ret->sign[(i << 1) + 1] = HEX (((unsigned char *) finded_pack->node->key)[i] & 0x0F);
    }

    switch (packitem_type) {
        case 0x10:
        ret->type = GIT_OBJ_TYPE_COMMIT;
        ret->ptr = __git_obj_transfer_commit (ret);
        break;
        case 0x20:
        ret->type = GIT_OBJ_TYPE_TREE;
        ret->ptr = __git_obj_transfer_tree (ret);
        break;
        case 0x30:
        ret->type = GIT_OBJ_TYPE_BLOB;
        ret->ptr = __git_obj_transfer_blob (ret);
        break;
        case 0x40:
        DBG_LOG (DBG_INFO, "UNDO");
        break;
        case 0x60:
        DBG_LOG (DBG_INFO, "OFS_DELTA");
        break;
        case 0x70:
        DBG_LOG (DBG_INFO, "REF_DELTA");
        break;
    }

    __gitpack_mmap_dispose (mmaped);
}

struct git_obj *__gitpack_obj_get__byte_string (struct __gitpack_collection *collection, const void *sign) {
    struct __gitpack_item_findret *findret = __gitpack_collection_rdtnode_find (collection, sign, rdt_find__byte_string);
    if (findret == NULL) return NULL;

    return __gitpack_obj_get (collection, findret);
}

struct git_obj *__gitpack_obj_get__char_string (struct __gitpack_collection *collection, const char *sign) {
    struct __gitpack_item_findret *findret = __gitpack_collection_rdtnode_find (collection, (const void *) sign, rdt_find__char_string);
    if (findret == NULL) return NULL;

    return __gitpack_obj_get (collection, findret);
}