#include "gitfs.h"
#include <malloc.h>

void __git_obj_dispose_delta (struct __git_obj_delta **head_ptr, struct __git_obj_delta **tail) {

}


void __git_obj_transfer_delta (struct git_obj *obj, struct __git_obj_delta **head_ptr, struct __git_obj_delta **tail_ptr) {
    if (obj == NULL) {
        DBG_LOG (DBG_ERROR, "__git_obj_transfer_delta: obj is null");
        return;
    }


    unsigned char *delta = obj->buf;
    unsigned long size = obj->size;

    printf ("%ld\n", size);

    while (delta < obj->body + obj->size) {
        unsigned char cmd = *delta++;
        if (cmd & 0x80) {
            unsigned long d_offset = 0;
            unsigned long d_size = 0;

            if (cmd & 0x01) d_offset = *delta++;
            if (cmd & 0x02) d_offset |= (*delta++ << 8);
            if (cmd & 0x04) d_offset |= (*delta++ << 16);
            if (cmd & 0x08) d_offset |= ((unsigned) *delta++ << 24);

            if (cmd & 0x10) d_size = *delta++;
            if (cmd & 0x20) d_size |= (*delta++ << 8);
            if (cmd & 0x40) d_size |= (*delta++ << 16);
            if (size == 0) d_size = 0x10000;

            size -= d_size;
        }
        else if (cmd) {
            if (cmd > size) {
                break;
            }
            printf ("%ld\n", size);
            delta += cmd;
            size -= cmd;
        }
        else {
            printf ("ERROR\n");
        }
    }

    printf ("CURRENT SIZE %ld\n", size);
}