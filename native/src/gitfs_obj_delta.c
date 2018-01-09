#include "gitfs.h"
#include <malloc.h>

void __git_obj_dispose_delta (struct __git_obj_delta **head_ptr, struct __git_obj_delta **tail) {

}

struct __git_obj_delta *__git_obj_transfer_delta_build_copy_ins (unsigned char *buf, int *buf_offset) {
    struct __git_obj_delta *delta = (struct __git_obj_delta *) malloc (sizeof (*delta));
    if (delta == NULL) {
        DBG_LOG (DBG_ERROR, "__git_obj_transfer_delta: have not enough free memory");
        return NULL;
    }

    delta->type = GIT_OBJ_DELTA_COPY;
    delta->ctx.copy.len = delta->ctx.copy.offset = 0;
    int j = *buf_offset;

    unsigned char cmd = buf[(*buf_offset)++];
    printf ("%02x\n", cmd);
    if (cmd & 0x01) delta->ctx.copy.offset = buf[(*buf_offset)++];
    if (cmd & 0x02) delta->ctx.copy.offset |= buf[(*buf_offset)++] << 8UL;
    if (cmd & 0x04) delta->ctx.copy.offset |= buf[(*buf_offset)++] << 16UL;
    if (cmd & 0x08) delta->ctx.copy.offset |= buf[(*buf_offset)++] << 24UL;

    if (cmd & 0x10) delta->ctx.copy.len = buf[(*buf_offset)++];
    if (cmd & 0x20) delta->ctx.copy.len |= buf[(*buf_offset)++] << 8UL;
    if (cmd & 0x40) delta->ctx.copy.len |= buf[(*buf_offset)++] << 16UL;
    if (delta->ctx.copy.len == 0) delta->ctx.copy.len = 0x10000;

    printf ("offset: %08x\tlen:%d\n", delta->ctx.copy.offset, delta->ctx.copy.len);

    return delta;
}

struct __git_obj_delta *__git_obj_transfer_delta_build_insert_ins (unsigned char *buf, int *buf_offset) {
    struct __git_obj_delta *delta = (struct __git_obj_delta *) malloc (sizeof (*delta));
    if (delta == NULL) {
        DBG_LOG (DBG_ERROR, "__git_obj_transfer_delta: have not enough free memory");
        return NULL;
    }

    unsigned char cmd = buf[(*buf_offset)++];

    delta->type = GIT_OBJ_DELTA_INSERT;
    delta->ctx.insert.len = cmd;
    delta->ctx.insert.start = buf + *buf_offset;
    delta->prev = delta->next = NULL;

    *buf_offset += cmd;
    return delta;
}

void __git_obj_transfer_delta (struct git_obj *obj, struct __git_obj_delta **head_ptr, struct __git_obj_delta **tail_ptr) {
    if (obj == NULL) {
        DBG_LOG (DBG_ERROR, "__git_obj_transfer_delta: obj is null");
        return;
    }

    int i = 0;
    while (i < obj->size) {
        struct __git_obj_delta *delta = NULL;
        if ((((unsigned char *) obj->buf)[i] & 0x80) != 0) {
            // this is a copy instruction
            delta = __git_obj_transfer_delta_build_copy_ins (obj->buf, &i);
        }
        else {
            // this is a insert instruction
            delta = __git_obj_transfer_delta_build_insert_ins (obj->buf, &i);
        }

        if (delta->type == GIT_OBJ_DELTA_INSERT) {
            int k;
            printf ("\n------------------------insert instruction:------------------------\n");
            for (k = 0; k < delta->ctx.insert.len; k++) {
                printf ("%c", ((char *)delta->ctx.insert.start)[k]);
            }
            printf ("\n------------------------end insert instruction------------------------\n");
        }
        else {
            printf ("\n!!!!!!!!!!!!!!!!!!!!!!!!copy instruction!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        }
        if (*head_ptr == NULL) {
            *head_ptr = delta;
        }
        if (*tail_ptr != NULL) {
            (*tail_ptr)->next = delta;
        }
        *tail_ptr = delta;
    }
}