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

struct __gitpack_file {
    int fd;
    size_t len;
};

struct __gitpack_file *__gitpack_fileopen (struct __gitpack *pack) {
    struct __gitpack_file *ret = (struct __gitpack_file *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_fileopen: have not enough free memory");
        return NULL;
    }
    ret->fd = open (pack->pack_path, O_RDONLY);
    if (ret->fd == -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_fileopen: cannot open fd");
        free (ret);
        return NULL;
    }

    struct stat st;
    if (fstat (ret->fd, &st) != 0) {
        close (ret->fd);
        free (ret);
        return NULL;
    }
    ret->len = st.st_size;

    return ret;
}

void __gitpack_file_dispose (struct __gitpack_file *obj) {
    if (obj == NULL) return;
    if (obj->fd != -1) close (obj->fd);
    free (obj);
}

void __gitpack_file_off (struct __gitpack_file *obj, size_t off) {
    if (obj == NULL) return;
    if (obj->fd == -1) return;

    lseek (obj->fd, off, SEEK_SET);
}

void *__gitpack_file_readbytes (struct __gitpack_file *obj, size_t size) {
    unsigned char *ret = (unsigned char *) malloc (size);
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_file_readbytes: have not enough free memory");
        return NULL;
    }
    read (obj->fd, ret, size);
    return ret;
}

unsigned char __gitpack_file_readbyte(struct __gitpack_file *obj) {
    char buf[1];
    read (obj->fd, buf, 1);
    return buf[0];
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

struct __gitpack_segment {
    struct __buf *buf;
    unsigned char type;
};

// get git object by pack
struct __gitpack_segment *__gitpack_segment_get (struct __gitpack_file *packfile, size_t off, size_t len) {
    __gitpack_file_off (packfile, off);

    unsigned char byte = __gitpack_file_readbyte (packfile);
    unsigned char type = (byte >> 4) & 0x07;

    int size = byte & 0x0F;
    int shift = 4;
    while (byte & 0x80) {
        byte = __gitpack_file_readbyte (packfile);
        size += ((int) (byte & 0x7F)) << shift;
        shift += 7;
    }
    if (type < 5) {
        struct __buf deflate_buf = { __gitpack_file_readbytes (packfile, size), len };
        struct __buf *inflated_buf = __inflate (&deflate_buf, size);
        free (deflate_buf.buf);
        
    }

    return NULL;
}

struct git_obj *__gitpack_obj_get__byte_string (struct git_repo *repo, const void *sign) {
    struct __gitpack_item_findret *findret = __gitpack_collection_rdtnode_find (repo->packes, sign, rdt_find__byte_string);
    if (findret == NULL) return NULL;

    struct __gitpack_file *packfile = __gitpack_fileopen (pack);
    if (packfile == NULL) return NULL;
    return __gitpack_segment_get (packfile, findret->node->off, findret->node->len);
}

struct git_obj *__gitpack_obj_get__char_string (struct git_repo *repo, const char *sign) {
    struct __gitpack_item_findret *findret = __gitpack_collection_rdtnode_find (repo->packes, (const void *) sign, rdt_find__char_string);
    if (findret == NULL) return NULL;

    struct __gitpack_file *packfile = __gitpack_fileopen (pack);
    if (packfile == NULL) return NULL;
    return __gitpack_segment_get (packfile, findret->node->off, findret->node->len);
}