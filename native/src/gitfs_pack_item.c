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
#include <errno.h>

// find node (segment's offset & len) by red black tree (idx)
struct rdt_node *__gitpack_find_rdtnode (pack, sign, find_func)
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
struct __gitpack_item_findret *__gitpack_collection_find_rdtnode (collection, sign, find_func)
    struct __gitpack_collection *collection;
    const void *sign;
    struct rdt_node *(*find_func) (const struct rdt *tree, const void *key);
{
    if (collection == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_collection_find_rdtnode: collection is null");
        return NULL;
    }
    struct __gitpack *cursor = collection->head;
    while (cursor != NULL) {
        struct rdt_node *findednode = __gitpack_find_rdtnode (cursor, sign, find_func);

        if (findednode != NULL) {
            struct __gitpack_item_findret *ret = (struct __gitpack_item_findret *) malloc (sizeof (*ret));
            if (ret == NULL) {
                DBG_LOG (DBG_ERROR, "__gitpack_collection_find_rdtnode: have not enough free memory");
                return NULL;
            }
            
            ret->node = findednode;
            ret->pack = cursor;

            return ret;
        }

        cursor = cursor->next;
    }
    return NULL;
}

struct __gitpack_file *__gitpack_fileopen (struct __gitpack *pack) {
    struct __gitpack_file *ret = (struct __gitpack_file *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_fileopen: have not enough free memory");
        return NULL;
    }
    ret->fd = open (pack->pack_path, O_RDONLY);
    if (ret->fd == -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_fileopen: cannot open fd");
        DBG_LOG (DBG_ERROR, strerror (errno));
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
    ret->pack = pack;

    return ret;
}

void __gitpack_file_dtor (struct __gitpack_file *obj) {
    if (obj == NULL) return;
    if (obj->fd != -1 && close (obj->fd) == -1) {
        DBG_LOG (DBG_ERROR, "__gitpack_file_dtor:");
        DBG_LOG (DBG_ERROR, strerror (errno));
    };
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

struct __gitpack_segment {
    struct __bytes bytes;
    unsigned char type;

    size_t item_len;
    size_t off;
};

// get git object by pack
struct __gitpack_segment *__gitpack_get_segment (struct __gitpack_file *packfile, size_t off, size_t len) {
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
        DBG_LOG (DBG_ERROR, "__gitpack_get_segment: have not enough free memory");
        return NULL;
    }

    if (len == 0) len = size * 3;

    ret->bytes.buf = __gitpack_file_readbytes (packfile, len);
    ret->bytes.len = len;
    ret->off = off;
    ret->item_len = size;
    ret->type = type;

    return ret;
}

void __gitpack_dtor_segment (struct __gitpack_segment *segment) {
    if (segment == NULL) return;
    if (segment->bytes.buf != NULL) free (segment->bytes.buf);
    free (segment);
}

struct __gitpack_item *__gitpack_get_item (struct __gitpack_segment segment) {
    struct __gitpack_item *ret = (struct __gitpack_item *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_item: have not enough free memory");
        return NULL;
    }
    ret->off = segment.off;
    ret->negative_off = 0;
    ret->base_sign = NULL;

    if (segment.type < 5) {
        struct __bytes deflate_obj = { segment.bytes.buf, segment.bytes.len };
        struct __bytes *tmp_store = __inflate (&deflate_obj, segment.item_len);
        ret->bytes = *tmp_store;
        free (tmp_store);
        ret->type = segment.type;
        return ret;
    }
    else if (segment.type == 6) {
        // ofs delta
        unsigned char *ptr = segment.bytes.buf;

        int nbytes = 1;
        ret->negative_off = *ptr & 0x7F;
        while (*ptr & 0x80) {
            ptr++;
            nbytes++;
            ret->negative_off = (ret->negative_off << 7) | (size_t) (*ptr & 0x7F);
        }
        if (nbytes >= 2) {
            int i;
            for (i = 1; i < nbytes; i++) ret->negative_off += (1 << (7 * i));
        }
        // printf ("[NBYTES] %d\n", nbytes);

        struct __bytes deflate_obj = { ptr + 1, segment.bytes.len - nbytes };
        struct __bytes *tmp_store = __inflate (&deflate_obj, segment.item_len);
        ret->bytes = *tmp_store;
        free (tmp_store);
        ret->type = segment.type;
        ret->origin_len = segment.item_len;

        return ret;
    }
    else if (segment.type == 7) {
        ret->base_sign = malloc (20);
        if (ret->base_sign == NULL) {
            DBG_LOG (DBG_ERROR, "__gitpack_get_item: have not enough free memory");
            free (ret);
            return NULL;
        }
        memcpy (ret->base_sign, segment.bytes.buf, 20);

        struct __bytes deflate_obj = { segment.bytes.buf + 20, segment.bytes.len - 20 };
        struct __bytes *tmp_store = __inflate (&deflate_obj, segment.item_len);
        ret->bytes = *tmp_store;
        free (tmp_store);
        ret->type = segment.type;
        ret->origin_len = segment.item_len;

        return ret;
    }
    
    DBG_LOG (DBG_ERROR, "__gitpack_get_item: cannot parse");
    return NULL;
}

void __gitpack_dtor_item (struct __gitpack_item *item) {
    if (item == NULL) return;
    if (item->base_sign != NULL) free (item->base_sign);
    // tips: this bytes.buf may a random number, because construct method have not initialize this field to 'NULL'
    if (item->bytes.buf != NULL) free (item->bytes.buf);
    free (item);
}

struct __gitpack_item *__gitpack_refdelta_patch (struct gitrepo *repo, struct __gitpack_item packitem) {
    struct __gitpack_item_findret *findret = __gitpack_collection_find_rdtnode (repo->packes, packitem.base_sign, rdt_find__byte_string);
    if (findret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_refdelta_patch: cannot find base item (from red black tree)");
        return NULL;
    }
    struct __gitpack_file *base_packfile = __gitpack_fileopen (findret->pack);
    if (base_packfile == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_refdelta_patch: cannot find packfile");
        free (findret);
        return NULL;
    }
    struct __gitpack_segment *base_segment = __gitpack_get_segment (base_packfile, findret->node->off, findret->node->len);
    if (base_segment == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_refdelta_patch: cannot get base segment");
        free (findret);
        __gitpack_file_dtor (base_packfile);
        return NULL;
    }
    struct __gitpack_item *base_packitem = __gitpack_get_item (*base_segment);
    __gitpack_dtor_segment (base_segment);

    if (base_packitem == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_refdelta_patch: cannot get base item");
        free (findret);
        __gitpack_file_dtor (base_packfile);
        return NULL;
    }

    if (base_packitem->type == 6) {
        struct __gitpack_item *tmp_packitem = __gitpack_ofsdelta_patch (repo, base_packfile, *base_packitem);

        __gitpack_dtor_item (base_packitem);
        base_packitem = tmp_packitem;
    }
    else if (base_packitem->type == 7) {
        struct __gitpack_item *tmp_packitem = __gitpack_refdelta_patch (repo, *base_packitem);

        __gitpack_dtor_item (base_packitem);
        base_packitem = tmp_packitem;
    }

    __gitpack_file_dtor (base_packfile);
    free (findret);

    if (base_packitem == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_refdelta_patch: cannot get base item");
        return NULL;
    }
    struct __gitpack_item *ret = (struct __gitpack_item *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_item: have not enough free memory");
        __gitpack_item_dtor (base_packitem);
        return NULL;
    }
    struct __bytes *patched_buf = __gitpack_delta_patch (base_packitem->bytes, packitem);
    __gitpack_item_dtor (base_packitem);
    if (patched_buf == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_refdelta_patch: cannot patch delta object");
        return NULL;
    }
    ret->bytes = *patched_buf;
    free (patched_buf);
    return ret;
}

size_t __gitpack_indexes_findlen__off (struct __gitpack_index* indexes, size_t count, size_t off) {
    int i = 0;
    int j = count;
    while (i < j) {
        int mid = (i + j) / 2;
        if (off < indexes[mid].off) j = mid - 1;
        else if (off > indexes[mid].off) i = mid + 1;
        else return indexes[mid].len;
    }
    return 0;
}

struct __gitpack_item *__gitpack_ofsdelta_patch (struct gitrepo *repo, struct __gitpack_file *packfile, struct __gitpack_item packitem) {
    // get base packitem1
    struct __gitpack_segment *base_segment = __gitpack_get_segment (
        packfile,
        packitem.off - packitem.negative_off,
        __gitpack_indexes_findlen__off (
            packfile->pack->indexes,
            packfile->pack->count,
            packitem.off - packitem.negative_off
        )
    );
    if (base_segment == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_item: cannot get base segment");
        return NULL;
    }
    struct __gitpack_item *base_packitem = __gitpack_get_item (*base_segment);
    // printf ("%d %d %d\n", packitem.off - packitem.negative_off, packitem.off, packitem.negative_off);
    __gitpack_dtor_segment (base_segment);
    
    if (base_packitem == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_item: cannot get base packitem");
        return NULL;
    }

    if (base_packitem->type == 6) {
        // if base item is ofs delta then find continue
        struct __gitpack_item *tmp_packitem = __gitpack_ofsdelta_patch (repo, packfile, *base_packitem);
        // exchange base packitem
        __gitpack_dtor_item (base_packitem);
        base_packitem = tmp_packitem;
    }
    else if (base_packitem->type == 7) {
        // if base item is ref delta then find continue
        struct __gitpack_item *tmp_packitem = __gitpack_refdelta_patch (repo, *base_packitem);
        // exchange base packitem
        __gitpack_dtor_item (base_packitem);
        base_packitem = tmp_packitem;
    }

    if (base_packitem == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_item: cannot get base packitem");
        return NULL;
    }

    struct __gitpack_item *ret = (struct __gitpack_item *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_item: have not enough free memory");
        __gitpack_dtor_item (base_packitem);
        return NULL;
    }
    struct __bytes *patched_buf = __gitpack_delta_patch (base_packitem->bytes, packitem);
    if (patched_buf == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_get_item: cannnot patch delta object");
        free (ret);
        __gitpack_dtor_item (base_packitem);
        return NULL;
    }

    ret->bytes = *patched_buf;
    free (patched_buf);
    ret->base_sign = NULL;
    ret->negative_off = 0;
    ret->off = 0;
    ret->origin_len = ret->bytes.len;
    ret->type = base_packitem->type;
    __gitpack_dtor_item (base_packitem);
    return ret;
}

struct gitobj *__gitpack_get_obj__common (struct gitrepo *repo, struct __gitpack_item_findret *findret) {
    struct __gitpack_file *packfile = __gitpack_fileopen (findret->pack);
    if (packfile == NULL) return NULL;
    struct __gitpack_segment *segment = __gitpack_get_segment (packfile, findret->node->off, findret->node->len);
    struct __gitpack_item *packitem = __gitpack_get_item (*segment);
    struct __gitpack_item *tmp_packitem = NULL;
    __gitpack_dtor_segment (segment);

    struct gitobj *ret = NULL;
    
    apply_delta:
    switch (packitem->type) {
        case 0x01:
            ret = __packitem_transfer_commit (*packitem);
            break;
        case 0x02:
            ret = __packitem_transfer_tree (*packitem);
            break;
        case 0x03:
            ret = __packitem_transfer_blob (*packitem);
            break;

        case 0x06:
            tmp_packitem = __gitpack_ofsdelta_patch (repo, packfile, *packitem);
            __gitpack_dtor_item (packitem);
            packitem = tmp_packitem;
            goto apply_delta;
        case 0x07:
            tmp_packitem = __gitpack_refdelta_patch (repo, *packitem);
            __gitpack_dtor_item (packitem);
            packitem = tmp_packitem;
            goto apply_delta;
    }

    __gitpack_dtor_file (packfile);
    free (packitem);

    // int i;
    // for (i = 0; i < ret->size; i++) printf ("%c", ret->body[i]);
    // fflush (stdout);

    return ret;
}

struct gitobj *__gitpack_getobj__bytestring (struct gitrepo *repo, const void *sign) {
    struct __gitpack_item_findret *findret = __gitpack_collection_find_rdtnode (repo->packes, sign, rdt_find__byte_string);
    if (findret == NULL) return NULL;

    struct gitobj *ret = __gitpack_get_obj__common (repo, findret);
    free (findret);
    return ret;
}

struct gitobj *__gitpack_getobj__charstring (struct gitrepo *repo, const char *sign) {
    if (repo == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_getobj_charstring: repo is null");
        return NULL;
    }
    if (sign == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_getobj__charstring: sign is null");
        return NULL;
    }
    if (strlen (sign) != 40) {
        DBG_LOG (DBG_ERROR, "__gitpack_getobj__charstring: sign length error");
        return NULL;
    }
    if (repo->packes == NULL) {
        DBG_LOG (DBG_INFO, "__gitpack_getobj__charstring: repo's packes is null");
        repo->packes = __gitpack_get_collection (repo);
    }

    struct __gitpack_item_findret *findret = __gitpack_collection_find_rdtnode (repo->packes, (const void *) sign, rdt_find__char_string);
    if (findret == NULL) return NULL;

    struct gitobj *ret = __gitpack_get_obj__common (repo, findret);
    free (findret);
    return ret;
}