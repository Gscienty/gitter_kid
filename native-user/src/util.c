#include "util.h"

struct __bytes *__bytes_ctor (size_t size) {
    struct __bytes *ret = (struct __bytes *) malloc (sizeof (struct __bytes));
    if (ret == NULL) {
        return NULL;
    }
    ret->len = size;
    if (size == 0) {
        ret->buf = NULL;
    }
    else {
        ret->buf = malloc (size);
    }

    return ret;
}

void __bytes_resize (struct __bytes *bytes, size_t size) {
    if (bytes == NULL) {
        return;
    }
    bytes->len = size;
    if (size == 0) {
        free (bytes->buf);
        bytes->buf = NULL;
    }
    else {
        bytes->buf = realloc(bytes->buf, size);
    }
}

void __bytes_dtor (struct __bytes *bytes) {
    if (bytes == NULL) {
        return;
    }

    if (bytes->buf != NULL) free (bytes->buf);
    free (bytes);
}