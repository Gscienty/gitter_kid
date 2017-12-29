#include "gitfs.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <zlib.h>

void *__zalloc (void *q, unsigned int n, unsigned int m) {
    q = NULL;

    return calloc (n, m);
}

void __zfree (void *q, void *p) {
    q = NULL;

    free (p);
}

char *__generate_obj_file_path (struct git_repo *repo, const char *signture) {
    int path_length = strlen (repo->path);
    char *ret = (char *) malloc (sizeof (char) * (path_length + 50));
    if (ret == NULL) {
        // have not enough free memory
        return NULL;
    }
    strcpy (ret, repo->path);
    strcpy (ret + path_length, "objects/");
    strncpy (ret + path_length + 8, signture, 2);
    strcpy (ret + path_length + 10, "/");
    strcpy (ret + path_length + 11, signture + 2);

    return ret;
}

struct __obj_file_ret {
    unsigned char *buf;
    int length;
};

struct __obj_file_ret *__get_repo_obj_by_sign (
    struct git_repo *repo,
    const char *signture,
    const char *obj_file_path) {

    if (obj_file_path == NULL) {
        return NULL;
    }

    struct __obj_file_ret *ret = (struct __obj_file_ret *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // have not enough free memory
        return NULL;
    }

    FILE *zipfile = fopen (obj_file_path, "rb");

    fseek (zipfile, 0, SEEK_END);
    int flen = ftell (zipfile);
    fseek (zipfile, 0, SEEK_SET);

    unsigned char *buf = malloc (sizeof (*buf) * flen);
    if (buf == NULL) {
        // have not enough free memory
        return NULL;
    }

    fread (buf, sizeof (unsigned char), flen, zipfile);

    ret->buf = buf;
    ret->length = flen;

    return ret;
}

struct __obj_file_ret *__inflate (struct __obj_file_ret *zip_buffer) {
    if (zip_buffer == NULL) {
        return NULL;
    }
    struct __obj_file_ret *ret = (struct __obj_file_ret *) malloc (sizeof (*ret));
    if (ret == NULL) {
        return NULL;
    }

    int init_buf_length = sizeof (unsigned char *) * (zip_buffer->length * 3 + 1024);
    ret->buf = (unsigned char *) malloc (sizeof (unsigned char) * init_buf_length);
    if (ret->buf == NULL) {
        // have not enough free memory
        free (ret);
        return NULL;
    }
    
    z_stream d_stream;
    d_stream.zalloc = __zalloc;
    d_stream.zfree = __zfree;
    d_stream.opaque = (voidpf) 0;
    d_stream.next_in = zip_buffer->buf;
    d_stream.avail_in = (unsigned int) zip_buffer->length;

    int err = inflateInit (&d_stream);
    d_stream.next_out = ret->buf;
    d_stream.avail_out = (unsigned int) init_buf_length;
    inflate (&d_stream, Z_NO_FLUSH);
    inflateEnd (&d_stream);

    ret->length = init_buf_length - d_stream.avail_out;

    return ret;
}

struct __obj_file_header {
    enum git_obj_type type;
    int length;
};

struct __obj_file_header *__get_git_obj_header (struct __obj_file_ret *inflated_buffer) {
    if (inflated_buffer == NULL) {
        return NULL;
    }
    struct __obj_file_header *ret = (struct __obj_file_header *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // not enough free memory
        return NULL;
    }
    char *header_line = strdup (inflated_buffer->buf);
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

struct git_obj *git_obj_get (struct git_repo *repo, const char* signture) {
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
    register char *sign_ch;
    for (sign_ch = (char *) signture; *sign_ch; sign_ch++) {
        if (!(('0' <= *sign_ch && *sign_ch <= '9') || ('a' <= *sign_ch && *sign_ch <= 'f'))) {
            // sign illegal
            return NULL;
        }
    }

    DBG_LOG (DBG_INFO, "git_obj_get: passed parameters check.");

    struct git_obj *ret = (struct git_obj *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // have not enough free memory
        DBG_LOG (DBG_ERROR, "git_obj_get: not enough free memory");
        return NULL;
    }
    char *obj_file_path = __generate_obj_file_path (repo, signture);
    if (obj_file_path == NULL) {
        //have not enough free memory
        DBG_LOG (DBG_ERROR, "git_obj_get: not enough free memory");
        free (ret);
        return NULL;
    }

    struct __obj_file_ret *obj_buffer = __get_repo_obj_by_sign (repo, signture, obj_file_path);
    if (obj_buffer == NULL) {
        // cannot get obj file. reason maybe have not enough memory or the object file not exist.
        DBG_LOG (DBG_ERROR, "git_obj_get: cannot get obj file");
        free (obj_file_path);
        free (ret);
        return NULL;
    }
    // inflate origin object file's content
    struct __obj_file_ret *inflated_buffer = __inflate (obj_buffer);
    // clear object file's used memory
    free (obj_buffer->buf);
    free (obj_buffer);
    if (inflated_buffer == NULL) {
        // cannot inflate buffer. reson maybe have not enough memory
        // or occur a error when inflating this buffer.
        DBG_LOG (DBG_ERROR, "git_obj_get: cannot inflate buffer");
        free (obj_file_path);
        free (ret);
        return NULL;
    }
    // try get object file header, which contains object file's type & length
    struct __obj_file_header *obj_header = __get_git_obj_header (inflated_buffer);
    if (obj_header == NULL || obj_header->type == GIT_OBJ_TYPE_UNKNOW) {
        // cannot recognize object header
        DBG_LOG (DBG_ERROR, "git_obj_get: cannot recognize object header");
        free (obj_file_path);
        free (ret);
        free (inflated_buffer->buf);
        free (inflated_buffer);
        if (obj_header != NULL) {
            free (obj_header);
        }
        return NULL;
    }

    ret->buf = inflated_buffer->buf;
    ret->path = obj_file_path;
    ret->sign = strdup (signture);
    ret->type = obj_header->type;
    ret->size = obj_header->length;
    // mark: body is sub-block of 'buf' (used to transfer to special obj type)
    ret->body = inflated_buffer->buf + strlen (inflated_buffer->buf) + 1;
    
    free (obj_header);
    free (inflated_buffer);

    switch (ret->type) {
        case GIT_OBJ_TYPE_BLOB:
            DBG_LOG (DBG_INFO, "git_obj_get: transfer to blob");
            ret->ptr = (void *) __git_obj_transfer_blob (ret);
            if (ret->ptr == NULL) {
                goto obj_type_occur_error;
            }
            break;
        case GIT_OBJ_TYPE_COMMIT:
            DBG_LOG (DBG_INFO, "git_obj_get: transfer to commit");
            ret->ptr = (void *) __git_obj_transfer_commit (ret);
            if (ret->ptr == NULL) {
                goto obj_type_occur_error;
            }
            break;
        case GIT_OBJ_TYPE_TREE:
            DBG_LOG (DBG_INFO, "git_obj_get: transfer to tree");
            ret->ptr = (void *) __git_obj_transfer_tree (ret);
            if (ret->ptr == NULL) {
                goto obj_type_occur_error;
            }
            break;
        default:
            obj_type_occur_error:
            DBG_LOG (DBG_ERROR, "git_obj_get: cannot transfer special object");
            // occur error.
            free (ret->buf);
            free (ret->path);
            free (ret->sign);
            free (ret);
            return NULL;
    }
    
    return ret;
}

enum git_obj_type git_obj_type (struct git_obj *obj) {
    return obj->type;
}

void git_obj_dispose (struct git_obj *obj) {
    if (obj == NULL) {
        return ;
    }
    switch (obj->type) {
        case GIT_OBJ_TYPE_BLOB:
            __git_obj_blob_dispose ((struct git_obj_blob *) obj->ptr);
            break;
        case GIT_OBJ_TYPE_COMMIT:
            __git_obj_commit_dispose ((struct git_obj_commit *) obj->ptr);
            break;
        case GIT_OBJ_TYPE_TREE:
            __git_obj_tree_dispose ((struct git_obj_tree *) obj->ptr);
            break;
    }

    free (obj->buf);
    free (obj->path);
    free (obj->sign);
    free (obj);
}