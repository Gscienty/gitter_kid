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

struct gitobj *__gitpack_wrap_obj (void *base, size_t len) {
    struct gitobj *ret = (struct gitobj *) malloc (sizeof (*ret));
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
    struct __buf buf;
    unsigned char type;

    size_t item_len;
    size_t off;
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
    struct __gitpack_segment *ret = (struct __gitpack_segment *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_segment_get: have not enough free memory");
        return NULL;
    }

    ret->buf.buf = __gitpack_file_readbytes (packfile, len);
    ret->buf.len = len;
    ret->off = off;
    ret->item_len = size;
    ret->type = type;

    return ret;
}

void __gitpack_segment_dispose (struct __gitpack_segment *segment) {
    if (segment == NULL) return;
    if (segment->buf.buf != NULL) free (segment->buf.buf);
    free (segment);
}

struct __gitpack_item {
    struct __buf buf;
    unsigned char type;

    void *base_sign;
    size_t negative_off;
    size_t off;
};

struct __gitpack_item *__gitpack_item_get (struct __gitpack_segment segment) {
    struct __gitpack_item *ret = (struct __gitpack_item *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_get: have not enough free memory");
        return NULL;
    }
    ret->off = segment.off;
    ret->negative_off = 0;
    ret->base_sign = NULL;

    if (segment.type < 5) {
        struct __buf deflate_obj = { segment.buf.buf, segment.buf.len };
        ret->buf = *__inflate (&deflate_obj, segment.item_len);
        ret->type = segment.type;
        return ret;
    }
    else if (segment.type == 6) {
        // ofs delta
        unsigned char *ptr = segment.buf.buf;

        int nbytes = 1;
        ret->negative_off = *ptr & 0x7F;
        while (*ptr & 0x80) {
            ptr++;
            nbytes++;
            ret->negative_off = (ret->negative_off << 7) + (size_t) (*ptr & 0x7F);
        }

        if (nbytes >= 2) {
            ret->negative_off += (1 << (7 * (nbytes - 1)));
        }

        struct __buf deflate_obj = { ptr + 1, segment.buf.len - nbytes };
        ret->buf = *__inflate (&deflate_obj, segment.item_len);
        ret->type = segment.type;

        return ret;
    }
    else if (segment.type == 7) {
        ret->base_sign = malloc (20);
        if (ret->base_sign == NULL) {
            DBG_LOG (DBG_ERROR, "__gitpack_item_get: have not enough free memory");
            free (ret);
            return NULL;
        }
        memcpy (ret->base_sign, segment.buf.buf, 20);

        struct __buf deflate_obj = { segment.buf.buf + 20, segment.buf.len - 20 };
        ret->buf = *__inflate (&deflate_obj, segment.item_len);
        ret->type = segment.type;

        return ret;
    }
    
    DBG_LOG (DBG_ERROR, "__gitpack_item_get: cannot parse");
    return NULL;
}

struct gitobj *__gitpack_item_transfer_commit (struct __gitpack_item item) {
    struct gitobj *ret = (struct gitobj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_transfer_commit: have not enough free memory");
        return NULL;
    }

    ret->buf = item.buf.buf;
    ret->path = NULL;
    ret->sign = NULL;
    ret->type = GIT_OBJ_TYPE_COMMIT;
    ret->size = item.buf.len;
    ret->body = item.buf.buf;
    ret->ptr = __git_obj_transfer_commit (item.buf.buf, item.buf.len);

    return ret;
}

struct gitobj *__gitpack_item_transfer_blob (struct __gitpack_item item) {
    struct gitobj *ret = (struct gitobj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_transfer_blob: have not enough free memory");
        return NULL;
    }

    ret->buf = item.buf.buf;
    ret->path = NULL;
    ret->sign = NULL;
    ret->type = GIT_OBJ_TYPE_BLOB;
    ret->size = item.buf.len;
    ret->body = item.buf.buf;
    ret->ptr = __git_obj_transfer_blob (item.buf.buf, item.buf.len);

    return ret;
}

struct gitobj *__gitpack_item_transfer_tree (struct __gitpack_item item) {
    struct gitobj *ret = (struct gitobj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_transfer_tree: have not enough free memory");
        return NULL;
    }

    ret->buf = item.buf.buf;
    ret->path = NULL;
    ret->sign = NULL;
    ret->type = GIT_OBJ_TYPE_TREE;
    ret->size = item.buf.len;
    ret->body = item.buf.buf;
    ret->ptr = __git_obj_transfer_tree (item.buf.buf, item.buf.len);

    return ret;
}

struct gitobj *__gitpack_obj_get__common (struct git_repo *repo, struct __gitpack_item_findret *findret) {
    struct __gitpack_file *packfile = __gitpack_fileopen (findret->pack);
    if (packfile == NULL) return NULL;
    struct __gitpack_segment *segment = __gitpack_segment_get (packfile, findret->node->off, findret->node->len);
    struct __gitpack_item *pack_item = __gitpack_item_get (*segment);
    __gitpack_segment_dispose (segment);

    switch (pack_item->type) {
        case 0x01: return __gitpack_item_transfer_commit (*pack_item);
        case 0x02: return __gitpack_item_transfer_tree (*pack_item);
        case 0x03: return __gitpack_item_transfer_blob (*pack_item);
    }
}

struct gitobj *__gitpack_obj_get__byte_string (struct git_repo *repo, const void *sign) {
    struct __gitpack_item_findret *findret = __gitpack_collection_rdtnode_find (repo->packes, sign, rdt_find__byte_string);
    if (findret == NULL) return NULL;

    return __gitpack_obj_get__common (repo, findret);
}

struct gitobj *__gitpack_obj_get__char_string (struct git_repo *repo, const char *sign) {
    struct __gitpack_item_findret *findret = __gitpack_collection_rdtnode_find (repo->packes, (const void *) sign, rdt_find__char_string);
    if (findret == NULL) return NULL;

    return __gitpack_obj_get__common (repo, findret);
}