#ifndef _G_KID_USER_UTIL_
#define _G_KID_USER_UTIL_

#include <malloc.h>

struct __bytes {
    size_t len;
    void *buf;
};

struct __bytes *__bytes_ctor (size_t size);
void __bytes_resize (struct __bytes *bytes, size_t size);
void __bytes_dtor (struct __bytes *bytes);

#endif