#include "util.h"
#include <zlib.h>
#include <malloc.h>

void *__zalloc (void *q, unsigned int n, unsigned int m) {
    q = NULL;

    return calloc (n, m);
}

void __zfree (void *q, void *p) {
    q = NULL;

    free (p);
}

// inflate object file
struct __bytes *__inflate (struct __bytes *deflate_bytes, size_t inflate_bytes_len) {
    if (deflate_bytes == NULL) return NULL;

    struct __bytes *ret = __bytes_ctor (inflate_bytes_len);
    if (ret == NULL) return NULL;
    
    z_stream d_stream;
    d_stream.zalloc = __zalloc;
    d_stream.zfree = __zfree;
    d_stream.opaque = (voidpf) 0;
    d_stream.next_in = deflate_bytes->buf;
    d_stream.avail_in = (unsigned int) deflate_bytes->len;

    int err = inflateInit (&d_stream);
    d_stream.next_out = ret->buf;
    d_stream.avail_out = (unsigned int) inflate_bytes_len;
    inflate (&d_stream, Z_NO_FLUSH);
    inflateEnd (&d_stream);
    ret->len = inflate_bytes_len - d_stream.avail_out;

    if (d_stream.avail_out != 0) {
        ret->buf = (unsigned char *) realloc (ret->buf, ret->len + 1);
        if (ret == NULL) {
            __bytes_dtor (ret);
            return NULL;
        }
        ret->buf[ret->len] = 0;
    }

    return ret;
}

struct __bytes *__bytes_ctor (size_t len) {
    struct __bytes *ret = (struct __bytes *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__bytes_ctor: have not enough free memory");
        return NULL;
    }
    ret->buf = (unsigned char *) malloc (len);
    if (ret->buf == NULL) {
        DBG_LOG (DBG_ERROR, "__bytes_ctor: have not enough free memory");
        free (ret);
        return NULL;
    }
    ret->len = len;
    return ret;
}

void __bytes_dtor (struct __bytes* bytes) {
    if (bytes == NULL) return;
    if (bytes->buf != NULL) free (bytes->buf);
    free (bytes);
}