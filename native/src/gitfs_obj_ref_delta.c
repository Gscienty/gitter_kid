#include "gitfs.h"
#include <string.h>
#include <malloc.h>

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