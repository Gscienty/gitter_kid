#include "gitfs.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

struct gitobj_blob *__git_obj_transfer_blob (char *body, size_t size) {
    struct gitobj_blob *ret = (struct gitobj_blob *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_INFO, "have not enough free memory");
        return NULL;
    }
    ret->len = size;
    ret->content = body;
    
    return ret;
}

struct gitobj *__gitpack_item_transfer_blob (struct __gitpack_item item) {
    struct gitobj *ret = (struct gitobj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_item_transfer_blob: have not enough free memory");
        return NULL;
    }

    ret->buf = item.bytes.buf;
    ret->path = NULL;
    ret->sign = NULL;
    ret->type = GIT_OBJ_TYPE_BLOB;
    ret->size = item.bytes.len;
    ret->body = item.bytes.buf;
    ret->ptr = __git_obj_transfer_blob (item.bytes.buf, item.bytes.len);

    return ret;
}

struct gitobj_blob *get_gitobj_blob (struct gitobj *obj) {
    if (obj == NULL) {
        return NULL;
    }
    if (obj->type == GIT_OBJ_TYPE_BLOB) {
        return (struct gitobj_blob *) obj->ptr;
    }
    else {
        return NULL;
    }
}

int gitobj_blob_length (struct gitobj_blob *blob_obj) {
    if (blob_obj == NULL) {
        return 0;
    }
    return blob_obj->len;
}

void *gitobj_blob_content (struct gitobj_blob *blob_obj) {
    if (blob_obj == NULL) {
        return 0;
    }
    return blob_obj->content;
}

void __gitobj_blob_dtor (struct gitobj_blob *obj) {
    if (obj == NULL) {
        return ;
    }

    free (obj);
}