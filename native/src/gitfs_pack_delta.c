#include "gitfs.h"
#include <malloc.h>
#include <string.h>

struct __buf *__gitpack_delta_patch (struct __gitpack_item base, struct __gitpack_item delta) {
    struct __buf *ret = (struct __buf *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_delta_patch: have not enough free memory");
        return NULL;
    }
    ret->len = 0;
    ret->buf = NULL;

    size_t delta_cur = 0;

    int i;
    for (i = 0; i < delta.buf.len; i++) printf ("%02x ", delta.buf.buf[i]); 
    printf ("\n");

    while (delta_cur < delta.buf.len) {
        unsigned char b = delta.buf.buf[delta_cur++];

        if (b & 0x80) {
            size_t base_off = 0;
            if (b & 0x01) base_off = base_off | ((unsigned int) delta.buf.buf[delta_cur++]);
            if (b & 0x02) base_off = base_off | (((unsigned int) delta.buf.buf[delta_cur++]) << 8);
            if (b & 0x04) base_off = base_off | (((unsigned int) delta.buf.buf[delta_cur++]) << 16);
            if (b & 0x08) base_off = base_off | (((unsigned int) delta.buf.buf[delta_cur++]) << 24);
            printf ("%08x\n", base_off);
            size_t bytes = 0;
            if (b & 0x10) bytes = bytes | ((unsigned int) delta.buf.buf[delta_cur++]);
            if (b & 0x20) bytes = bytes | (((unsigned int) delta.buf.buf[delta_cur++]) << 8);
            if (b & 0x40) bytes = bytes | (((unsigned int) delta.buf.buf[delta_cur++]) << 16);

            if (bytes == 0) bytes = 0x10000;
            
            if (ret->buf == NULL) ret->buf = malloc (bytes);
            else ret->buf = realloc (ret->buf, ret->len + bytes);
            if (ret->buf == NULL) {
                DBG_LOG (DBG_ERROR, "__gitpack_delta_patch: have not enough free memory");
                free (ret);
                return NULL;
            }

            memcpy (ret->buf + ret->len, base.buf.buf + base_off, bytes);
            ret->len += bytes;
        }
        else {
            if (b == 0) return NULL;

            size_t bytes = b;

            if (ret->buf == NULL) ret->buf = malloc (bytes);
            else ret->buf = realloc (ret->buf, ret->len + bytes);
            if (ret->buf == NULL) {
                DBG_LOG (DBG_ERROR, "__gitpack_delta_patch: have not enough free memory");
                free (ret);
                return NULL;
            }

            memcpy (ret->buf + ret->len, delta.buf.buf + delta_cur, bytes);

            ret->len += bytes;
            delta_cur += bytes;
        }

        return ret;
    }
}