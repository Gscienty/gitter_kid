#include "gitfs.h"
#include <malloc.h>
#include <string.h>

struct __buf *__gitpack_delta_patch (struct __gitpack_item base, struct __gitpack_item delta) {
    // printf ("%02x %02x\n", delta.buf.buf[0], delta.buf.buf[1]);
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

            // printf ("COPY basesize:%d\toff:%d\tsize:%d:\n", base.origin_len, cp_off, cp_size);
            // int i;
            // for (i = 0; i < cp_size; i++) printf ("%c", *(char *) (base.buf.buf + cp_off + i));
            // printf ("\n");

            memcpy (out, (char *) base.buf.buf + cp_off, cp_size);
            out += cp_size;
            size -= cp_size;
        }
        else if (cmd) {
            if (cmd > size) break;
            
            // printf ("INSERT:\n");
            // int i;
            // for (i = 0; i < cmd; i++) printf ("%c", *(char *) (data + i));
            // printf ("\n");

            memcpy (out, data, cmd);
            out += cmd;
            data += cmd;
            size -= cmd;
        }
        else {

        }
    }

    return ret;
}