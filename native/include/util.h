#ifndef _G_KID_UTIL_
#define _G_KID_UTIL_

#include "define.h"

struct __bytes {
    unsigned char *buf;
    int len;
};

struct __bytes *__inflate (struct __bytes *deflate_bytes, size_t inflate_bytes_len);
struct __bytes *__bytes_ctor (size_t len);
void __bytes_dtor (struct __bytes* bytes);
#endif