#include "gitfs.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

struct git_obj_blob *__git_obj_transfer_blob (struct git_obj *obj) {
    struct git_obj_blob *ret = (struct git_obj_blob *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_INFO, "have not enough free memory");
        return NULL;
    }
    ret->length = obj->size;
    ret->content = obj->body;
    
    return ret;
}

struct git_obj_blob *git_obj_get_blob (struct git_obj *obj) {
    if (obj == NULL) {
        return NULL;
    }
    if (obj->type == GIT_OBJ_TYPE_BLOB) {
        return (struct git_obj_blob *) obj->ptr;
    }
    else {
        return NULL;
    }
}

int git_obj_blob_length (struct git_obj_blob *blob_obj) {
    if (blob_obj == NULL) {
        return 0;
    }
    return blob_obj->length;
}

void *git_obj_blob_content (struct git_obj_blob *blob_obj) {
    if (blob_obj == NULL) {
        return 0;
    }
    return blob_obj->content;
}

void __git_obj_blob_dispose (struct git_obj_blob *obj) {
    if (obj == NULL) {
        return ;
    }

    free (obj);
}