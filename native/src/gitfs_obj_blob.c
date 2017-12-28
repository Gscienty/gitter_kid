#include "gitfs.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

struct git_obj_blob *git_obj_get_blob (struct git_obj *obj) {
    struct git_obj_blob *ret = (struct git_obj_blob *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_INFO, "have not enough free memory");
        return NULL;
    }
    ret->length = obj->size;
    ret->content = obj->body;
    
    return ret;
}