#include "gitfs.h"
#include <malloc.h>
#include <string.h>

struct __buf *__gitpack_delta_patch (struct __buf base, struct __gitpack_item delta) {
    unsigned char *data = delta.buf.buf + 2;
    const unsigned char *top = (const unsigned char *) delta.buf.buf + delta.buf.len;

    size_t size = 0;

    int i = 0;
    unsigned long tmp;
    do {
        tmp = *data++;
        size |= (tmp & 0x7f) << i;
        i += 7;
    } while (tmp & 0x80 && data < top);
    // printf ("[RESULT] size: %d\n", size);
    // printf ("[BASE] size: %d\n", base.len);
    // printf ("%s\n", base.buf);
    struct __buf *ret = (struct __buf *) malloc (sizeof (*ret));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_delta_patch: have not enough free memory");
        return NULL;
    }
    ret->len = size;
    ret->buf = (unsigned char *) malloc (size);
    if (ret->buf == NULL) {
        DBG_LOG (DBG_ERROR, "__gitpack_delta_patch: have not enough free memory");
        free (ret);
        return NULL;
    }

    char *out = ret->buf;
    while (data < top) {
        unsigned char cmd = *data++;
        if (cmd & 0x80) {
            unsigned long cp_off = 0;
            unsigned long cp_size = 0;
            if (cmd & 0x01) cp_off = *data++;
            if (cmd & 0x02) cp_off |= (*data++ << 8);
            if (cmd & 0x04) cp_off |= (*data++ << 16);
            if (cmd & 0x08) cp_off |= ((unsigned) *data++ << 24);
            if (cmd & 0x10) cp_size = *data++;
            if (cmd & 0x20) cp_size |= (*data++ << 8);
            if (cmd & 0x40) cp_size |= (*data++ << 16);
            if (cp_size == 0) cp_size = 0x10000;

            // printf ("[COPY] off: %ld,\tsize: %ld\n", cp_off, cp_size);

            memcpy (out, (char *) base.buf + cp_off, cp_size);
            out += cp_size;
            size -= cp_size;
        }
        else if (cmd) {
            if (cmd > size) break;
            // printf ("[INSERT] size:%d\n", cmd);
            // for (i = 0; i < cmd; i++) printf ("%c", data[i]);
            // printf ("\n");
            memcpy (out, data, cmd);
            out += cmd;
            data += cmd;
            size -= cmd;
        }
        else {

        }
    }

    if (data != top || size != 0) {
        DBG_LOG (DBG_ERROR, "__gitpack_delta_patch: format error");
        // printf ("[INNER] top - data: %d\t size: %d\n", top - data, size);
    }

    // for (i = 0; i < ret->len; i++) printf ("%c", ret->buf[i]);
    // printf ("\n");

    return ret;
}