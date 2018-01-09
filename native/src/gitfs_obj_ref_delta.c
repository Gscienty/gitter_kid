#include "gitfs.h"
#include <string.h>
#include <malloc.h>

struct git_obj *__git_packitem_transfer_ref_delta_build_git_obj (struct __git_packitem *packitem, const char *signture) {
    struct git_obj *ret = (struct git_obj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_transfer_ref_delta: have not enough free memory");
        return NULL;
    }
    struct __obj_file_ret *inflated_obj = __git_packitem_inflate (packitem);
    ret->buf = inflated_obj->buf;
    ret->path = NULL;
    ret->sign = strdup (signture);
    ret->type = GIT_OBJ_TYPE_REF_DELTA;
    ret->ptr = NULL;
    ret->size = inflated_obj->len;
    ret->body = inflated_obj->buf;
    free (inflated_obj);

    return ret;
}

struct git_obj *__git_packitem_transfer_ref_delta (struct __git_packitem *packitem, const char *signture) {
    struct git_obj_ref_delta *ref_delta = (struct git_obj_ref_delta *) malloc (sizeof (*ref_delta));
    if (ref_delta == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_transfer_ref_delta: have not enough free memory");
        return NULL;
    }
    // get base sign
    ref_delta->base_sign = (void *) malloc (20);
    if (ref_delta->base_sign == NULL) {
        DBG_LOG (DBG_ERROR, "__git_packitem_transfer_ref_delta: have not enough free memory");
        free (ref_delta);
        return NULL;
    }
    memcpy (ref_delta->base_sign, packitem->mmaped_base + packitem->inner_offset, 20);
    packitem->inner_offset += 20;

    // wrapped to git_obj
    struct git_obj *ret = __git_packitem_transfer_ref_delta_build_git_obj (packitem, signture);

    ref_delta->head = ref_delta->tail = ref_delta->cursor = NULL;
    __git_obj_transfer_delta (ret, &ref_delta->head, &ref_delta->tail);
    
    ret->ptr = ref_delta;
    return ret;
}