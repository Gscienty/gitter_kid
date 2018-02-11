#include "util.h"
#include <zlib.h>
#include <malloc.h>
#include <string.h>

void *__zalloc (void *q, unsigned int n, unsigned int m) {
    q = NULL;

    return calloc (n, m);
}

void __zfree (void *q, void *p) {
    q = NULL;

    free (p);
}

struct __bytes *__bytes_ctor (size_t len) {
    struct __bytes *ret = (struct __bytes *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__bytes_ctor: have not enough free memory");
        return NULL;
    }
    if (len == 0) {
        ret->buf = NULL;
    }
    else {
        ret->buf = (unsigned char *) malloc (len);
        if (ret->buf == NULL) {
            DBG_LOG (DBG_ERROR, "__bytes_ctor: have not enough free memory");
            free (ret);
            return NULL;
        }
    }

    ret->len = len;
    return ret;
}

void __bytes_dtor (struct __bytes* bytes) {
    if (bytes == NULL) return;
    if (bytes->buf != NULL) free (bytes->buf);
    free (bytes);
}

void __bytes_resize (struct __bytes *bytes, size_t len) {
    if (bytes == NULL) {
        DBG_LOG (DBG_ERROR, "__bytes_resize: bytes is null");
        return ;
    }
    if (len == 0) {
        free (bytes->buf);
        bytes->buf = NULL;
    }
    else if (bytes->len == 0) {
        bytes->buf = malloc (len);
    }
    else {
        bytes->buf = realloc (bytes->buf, len);
    }

    if (bytes->buf == NULL && len != 0) {
        DBG_LOG (DBG_ERROR, "__bytes_resize: have not free memory");
    }

    bytes->len = len;
}

// inflate object file
struct __bytes *__inflate (struct __bytes *deflate_bytes, size_t inflate_bytes_len) {
    if (deflate_bytes == NULL) {
        DBG_LOG (DBG_ERROR, "__inflate: deflate_bytes is null");
        return NULL;
    }
    struct __bytes *ret = __bytes_ctor (0);
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__inflate: have not free memory");
        return NULL;
    }

    unsigned char *inflate_buf = (unsigned char *) malloc (inflate_bytes_len);
    if (inflate_buf == NULL) {
        DBG_LOG (DBG_ERROR, "__inflate: have not free memory");
        __bytes_dtor (ret);
        return NULL;
    }

    z_stream inflated_stream;
    inflated_stream.zalloc = __zalloc;
    inflated_stream.zfree = __zfree;
    inflated_stream.opaque = NULL;
    inflated_stream.avail_in = (unsigned int) deflate_bytes->len;
    inflated_stream.next_in = deflate_bytes->buf;

    int retval = inflateInit (&inflated_stream);
    if (retval != Z_OK) {
        DBG_LOG (DBG_ERROR, "__inflate: initialize inflate error");
        __bytes_dtor (ret);
        free (inflate_buf);
        return NULL;
    }

    size_t total_size = 0;
    do {
        inflated_stream.avail_out = inflate_bytes_len;
        inflated_stream.next_out = inflate_buf;
        retval = inflate (&inflated_stream, Z_NO_FLUSH);
        switch (retval) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                DBG_LOG (DBG_ERROR, "__inflate: inflate error");
                inflateEnd (&inflated_stream);
                free (inflate_buf);
                __bytes_dtor (ret);
                return NULL;
        }

        size_t have = inflate_bytes_len - inflated_stream.avail_out;
        total_size += have;
        __bytes_resize (ret, total_size);

        memcpy (ret->buf + ret->len - have, inflate_buf, have);
    } while (inflated_stream.avail_out == 0);

    inflateEnd (&inflated_stream);
    free (inflate_buf);

    return ret;
}
