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


struct __git_packitem *__git_packitem_mmap (struct git_pack *pack, int packfile_offset, int pack_len) {
    struct __git_packitem *result = (struct __git_packitem *) malloc (sizeof (*result));
    if (result == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_mmap: have not enough free memory");
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
    result->pack_fd = __git_get_packfd (pack);
    if (result->pack_fd == -1) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_obj: cannot open pack file");
        return NULL;
    }

    // mmap file's sub-content
    result->mmaped_base = mmap (NULL, result->mmaped_len, PROT_READ, MAP_SHARED, result->pack_fd, page_size * nth_page);
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
    if (mmaped->pack_fd != 0) {
        close (mmaped->pack_fd);
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
    
    struct __obj_file_ret *inflated_obj = __inflate (deflated_obj, packitem->origin_len);
    
    free (deflated_obj);

    return inflated_obj;
}

struct git_obj *__git_packitem_transfer_ref_delta (struct __git_packitem *packitem, const char *signture) {
    struct git_obj *ret = (struct git_obj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_transfer_ref_delta: have not enough free memory");
        return NULL;
    }
    struct git_obj_ref_delta *ref_delta = (struct git_obj_ref_delta *) malloc (sizeof (*ref_delta));
    if (ref_delta == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_transfer_ref_delta: have not enough free memory");
        free (ret);
        return NULL;
    }
    ref_delta->base_sign = (void *) malloc (20);
    if (ref_delta->base_sign == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_transfer_ref_delta: have not enough free memory");
        free (ret);
        free (ref_delta);
        return NULL;
    }
    memcpy (ref_delta->base_sign, packitem->mmaped_base + packitem->inner_offset, 20);
    packitem->inner_offset += 20;

    struct __obj_file_ret *inflated_obj = __git_packitem_inflate (packitem);
    ref_delta->content = inflated_obj->buf;
    ref_delta->len = inflated_obj->len;
    free (inflated_obj);

    ret->buf = ref_delta->content;
    ret->path = NULL;
    ret->sign = strdup (signture);
    ret->type = GIT_OBJ_TYPE_REF_DELTA;
    ret->ptr = ref_delta;
    ret->size = ref_delta->len;
    ret->body = ref_delta->content;

    return ret;
}

struct git_obj *__git_packitem_transfer_ofs_delta (struct __git_packitem *packitem, const char *signture) {
    struct git_obj *ret = (struct git_obj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_transfer_ofs_delta: have not enough free memory");
        return NULL;
    }
    struct git_obj_ofs_delta *ofs_delta = (struct git_obj_ofs_delta *) malloc (sizeof (*ofs_delta));
    if (ofs_delta == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_transfer_ofs_delta: have not enough free memory");
        free (ret);
        return NULL;
    }
    unsigned int offset = 0x00000000;
    unsigned int mask = 0x00000000;
    int i;
    for (i = 0; i < packitem->_n - 1; i++) {
        mask |= 0x7F << (7 * i);
        offset |= ((*(unsigned char *) (packitem->mmaped_base + packitem->inner_offset + i)) & 0x7F) << (7 * i);
    }
    mask |= 0xFF << (7 * (packitem->_n - 1));
    offset |= ((*(unsigned char *) (packitem->mmaped_base + packitem->inner_offset + packitem->_n - 1)) & 0x7F) << (7 * (packitem->_n - 1));
    offset = ~mask | offset;
    ofs_delta->offset = offset;

    packitem->inner_offset += packitem->_n;
    struct __obj_file_ret *inflated_obj = __git_packitem_inflate (packitem);
    ofs_delta->content = inflated_obj->buf;
    ofs_delta->len = inflated_obj->len;
    free (inflated_obj);

    ret->buf = ofs_delta->content;
    ret->path = NULL;
    ret->sign = strdup (signture);
    ret->type = GIT_OBJ_TYPE_OFS_DELTA;
    ret->ptr = ofs_delta;
    ret->size = ofs_delta->len;
    ret->body = ofs_delta->content;

    return ret;
}

struct __git_packitem *__git_pack_get_packitem (struct git_pack *pack, const char *signture) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_packitem: pack is null");
        return NULL;
    }
    if (signture == NULL || strlen (signture) != 40) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_packitem: signture illegal");
        return NULL;
    }

    // find idx's node what contains pack item's offset & len by red black tree
    struct rdt_node *node = rdt_find (pack->rdtree, signture);
    if (node == NULL) {
        DBG_LOG (DBG_INFO, "__git_pack_get_packitem: have not finded entry");
        return NULL;
    }

    // get deflated pack item
    struct __git_packitem *packitem = __git_packitem_mmap (pack, node->offset, node->len);
    if (packitem == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_packitem: cannot get pack item");
        return NULL;
    }

    return packitem;
}

struct git_obj *__git_pack_get_obj (struct git_pack *pack, const char *signture) {
    struct __git_packitem *packitem = __git_pack_get_packitem (pack, signture);
    if (packitem == NULL) {
        return NULL;
    }

    struct git_obj *ret = NULL;
    // get object type
    switch (packitem->item_type) {
        case 0x10: // GIT_OBJ_TYPE_COMMIT;
            break;
        case 0x20: // GIT_OBJ_TYPE_TREE;
            break;
        case 0x30: // GIT_OBJ_TYPE_BLOB;
            break;
        case 0x40: // GIT_OBJ_TYPE_TAG;
            break;
        case 0x60: // GIT_OBJ_TYPE_OFS_DELTA;
            ret = __git_packitem_transfer_ofs_delta (packitem, signture);
            printf ("OFS: %s\n", (char *) ((struct git_obj_ofs_delta *) ret->ptr)->content);
            break;
        case 0x70: // GIT_OBJ_TYPE_REF_DELTA;
            ret = __git_packitem_transfer_ref_delta (packitem, signture);
            printf ("REF: %s\n", (char *) ((struct git_obj_ref_delta *) ret->ptr)->content);
            break;
        default: ret->type = GIT_OBJ_TYPE_UNKNOW; break;
    }

    __git_packitem_dispose (packitem);

    return ret;
}