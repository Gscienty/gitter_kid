#include "gitfs.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <zlib.h>
#include <unistd.h>

void *__zalloc (void *q, unsigned int n, unsigned int m) {
    q = NULL;

    return calloc (n, m);
}

void __zfree (void *q, void *p) {
    q = NULL;

    free (p);
}

// inflate object file
struct __buf *__inflate (struct __buf *zip_buffer, int inflated_buffer_len) {
    if (zip_buffer == NULL) {
        return NULL;
    }
    struct __buf *ret = (struct __buf *) malloc (sizeof (*ret));
    if (ret == NULL) {
        return NULL;
    }

    ret->buf = (unsigned char *) malloc (sizeof (unsigned char) * inflated_buffer_len);
    if (ret->buf == NULL) {
        // have not enough free memory
        DBG_LOG (DBG_ERROR, "__inflate: have not enough free memory");
        free (ret);
        return NULL;
    }
    
    z_stream d_stream;
    d_stream.zalloc = __zalloc;
    d_stream.zfree = __zfree;
    d_stream.opaque = (voidpf) 0;
    d_stream.next_in = zip_buffer->buf;
    d_stream.avail_in = (unsigned int) zip_buffer->len;

    int err = inflateInit (&d_stream);
    d_stream.next_out = ret->buf;
    d_stream.avail_out = (unsigned int) inflated_buffer_len;
    inflate (&d_stream, Z_NO_FLUSH);
    inflateEnd (&d_stream);

    ret->len = inflated_buffer_len - d_stream.avail_out;

    return ret;
}

char *__gitobj_path_get (struct git_repo *repo, const char *signture) {
    size_t path_length = strlen (repo->path);
    char *ret = malloc (path_length + 128);
    if (ret == NULL) {
        // have not enough free memory
        return NULL;
    }
    snprintf (ret, path_length + 50, "%sobjects/%c%c/%s", repo->path, signture[0], signture[1], signture + 2);
    
    if (access (ret, F_OK) != 0) {
        free (ret);
        return NULL;
    }

    return ret;
}


// object file stored by loose type. get object content
struct __buf *__gitobj_loose_content (FILE *object_file) {
    struct __buf *ret = (struct __buf *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // have not enough free memory
        DBG_LOG (DBG_ERROR, "__gitobj_loose_content: have not enough free memory");
        return NULL;
    }

    fseek (object_file, 0, SEEK_END);
    int flen = ftell (object_file);
    fseek (object_file, 0, SEEK_SET);

    unsigned char *buf = malloc (sizeof (*buf) * flen);
    if (buf == NULL) {
        // have not enough free memory
        return NULL;
    }

    fread (buf, sizeof (unsigned char), flen, object_file);

    ret->buf = buf;
    ret->len = flen;

    return ret;
}

// get repository's object file
struct __buf *__gitobj_get (const char *path) {
    if (path == NULL) {
        DBG_LOG (DBG_ERROR, "__gitobj_get: object file's path is null");
        return NULL;
    }

    FILE *object_file = fopen (path, "rb");
    if (object_file == NULL) {
        DBG_LOG (DBG_ERROR, "__gitobj_get: cannot open object file");
        return NULL;
    }
    struct __buf *ret = __gitobj_loose_content (object_file);

    fclose (object_file);
    
    return ret;
}

struct __gitobj_header {
    enum gitobj_type type;
    int length;
};

struct __gitobj_header *__gitobj_header_get (unsigned char *buf) {
    if (buf == NULL) {
        return NULL;
    }
    struct __gitobj_header *ret = (struct __gitobj_header *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // not enough free memory
        return NULL;
    }
    char *header_line = strdup (buf);
    if (header_line == NULL) {
        // header illegal
        free (ret);
        return NULL;
    }
    char *space_ptr = strrchr (header_line, ' ');
    if (space_ptr == NULL) {
        // header illegal
        free (header_line);
        free (ret);
        return NULL;
    }
    *space_ptr = 0;
    char *ep;
    ret->length = strtol (space_ptr + 1, &ep, 10);
    ret->type = GIT_OBJ_TYPE_UNKNOW;
    if (strcmp (header_line, "blob") == 0) {
        ret->type = GIT_OBJ_TYPE_BLOB;
    }
    else if (strcmp (header_line, "commit") == 0) {
        ret->type = GIT_OBJ_TYPE_COMMIT;
    }
    else if (strcmp (header_line, "tree") == 0) {
        ret->type = GIT_OBJ_TYPE_TREE;
    }
    free (header_line);

    return ret;
}

struct __buf *__gitobj_get_content (const char *path) {
    struct __buf *deflated_obj = __gitobj_get (path);
    if (deflated_obj == NULL) {
        // cannot get obj file. reason maybe have not enough memory or the object file not exist.
        DBG_LOG (DBG_ERROR, "get_gitobj: cannot get obj file");
        return NULL;
    }
    // inflate origin object file's content
    struct __buf *inflated_buffer = __inflate (deflated_obj, deflated_obj->len * 3 + 1024);
    // clear object file's used memory
    free (deflated_obj->buf);
    free (deflated_obj);
    if (inflated_buffer == NULL) {
        // cannot inflate buffer. reson maybe have not enough memory
        // or occur a error when inflating this buffer.
        DBG_LOG (DBG_ERROR, "get_gitobj: cannot inflate buffer");
        return NULL;
    }
}

struct gitobj *__gitobj_loose_get (const char *obj_path, const char *signture) {
    struct gitobj *ret = (struct gitobj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // have not enough free memory
        DBG_LOG (DBG_ERROR, "get_gitobj: not enough free memory");
        return NULL;
    }

    struct __buf *inflated_buffer = __gitobj_get_content (obj_path);
    if (inflated_buffer == NULL) return NULL;

    // try get object file header, which contains object file's type & length
    struct __gitobj_header *obj_header = __gitobj_header_get (inflated_buffer->buf);
    if (obj_header == NULL || obj_header->type == GIT_OBJ_TYPE_UNKNOW) {
        // cannot recognize object header
        DBG_LOG (DBG_ERROR, "get_gitobj: cannot recognize object header");
        free (ret);
        free (inflated_buffer->buf);
        free (inflated_buffer);
        if (obj_header != NULL) free (obj_header);        
        return NULL;
    }
    
    ret->buf = inflated_buffer->buf;
    ret->path = (char *) obj_path;
    ret->sign = strdup (signture);
    ret->type = obj_header->type;
    ret->size = obj_header->length;
    // mark: body is sub-block of 'buf' (used to transfer to special obj type)
    ret->body = inflated_buffer->buf + strlen (inflated_buffer->buf) + 1;
    
    free (obj_header);
    free (inflated_buffer);

    switch (ret->type) {
        case GIT_OBJ_TYPE_BLOB:
            DBG_LOG (DBG_INFO, "get_gitobj: transfer to blob");
            ret->ptr = (void *) __git_obj_transfer_blob (ret->body, ret->size);
            if (ret->ptr == NULL) {
                goto obj_type_occur_error;
            }
            break;
        case GIT_OBJ_TYPE_COMMIT:
            DBG_LOG (DBG_INFO, "get_gitobj: transfer to commit");
            ret->ptr = (void *) __git_obj_transfer_commit (ret->body, ret->size);
            if (ret->ptr == NULL) {
                goto obj_type_occur_error;
            }
            break;
        case GIT_OBJ_TYPE_TREE:
            DBG_LOG (DBG_INFO, "get_gitobj: transfer to tree");
            ret->ptr = (void *) __git_obj_transfer_tree (ret->body, ret->size);
            if (ret->ptr == NULL) {
                goto obj_type_occur_error;
            }
            break;
        default:
            obj_type_occur_error:
            DBG_LOG (DBG_ERROR, "get_gitobj: cannot transfer special object");
            // occur error.
            free (ret->buf);
            free (ret->path);
            free (ret->sign);
            free (ret);
            return NULL;
    }
    return ret;
}

struct gitobj *get_gitobj (struct git_repo *repo, const char* signture) {
    if (repo == NULL) {
        return NULL;
    }
    if (signture == NULL) {
        return NULL;
    }
    if (strlen (signture) != 40) {
        // sign illegal
        return NULL;
    }
    char *sign_ch;
    for (sign_ch = (char *) signture; *sign_ch; sign_ch++) {
        if (!(('0' <= *sign_ch && *sign_ch <= '9') || ('a' <= *sign_ch && *sign_ch <= 'f'))) {
            // sign illegal
            return NULL;
        }
    }
    
    char *obj_path = __gitobj_path_get (repo, signture);

    if (obj_path == NULL) {
        if (repo->packes == NULL) repo->packes = __gitpack_collection_get (repo);
        return __gitpack_getobj__charstring (repo, signture);
    }
    else{
         // get obj by loose
        struct gitobj *ret = __gitobj_loose_get (obj_path, signture);
        free (obj_path);
        return ret;
    }
}

enum gitobj_type get_gitobj_type (struct gitobj *obj) {
    return obj->type;
}

void dispose_gitobj (struct gitobj *obj) {
    if (obj == NULL) {
        return ;
    }
    switch (obj->type) {
        case GIT_OBJ_TYPE_BLOB:
            __gitobj_blob_dispose ((struct gitobj_blob *) obj->ptr);
            break;
        case GIT_OBJ_TYPE_COMMIT:
            __gitobj_commit_dispose ((struct gitobj_commit *) obj->ptr);
            break;
        case GIT_OBJ_TYPE_TREE:
            __gitobj_tree_dispose ((struct gitobj_tree *) obj->ptr);
            break;
    }

    free (obj->buf);
    if (obj->path != NULL) {
        free (obj->path);
    }
    free (obj->sign);
    free (obj);
}