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

void __gitobj_blob_dispose (struct gitobj_blob *obj) {
    if (obj == NULL) {
        return ;
    }

    free (obj);
}