#include "gitfs.h"
#include <string.h>
#include <malloc.h>

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
    int i;
    for (i = 0; i < packitem->_n - 1; i++) {
        offset = (offset << 8) | 0x80 | ((*(unsigned char *) (packitem->mmaped_base + packitem->inner_offset + i)) & 0x7F);
    }
    offset = (offset << 8) | ((*(unsigned char *) (packitem->mmaped_base + packitem->inner_offset + packitem->_n - 1)) & 0x8F);
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