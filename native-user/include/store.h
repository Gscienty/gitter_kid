#ifndef _G_KID_USER_STORE_
#define _G_KID_USER_STORE_
#include "define.h"

struct __store_fields {
    char *path;
    int fd;
};

struct __store_operations {
    int (*open_store) ();
    void *(*store_read_data) ();
    void (*store_data_dtor) (void *);
};

struct __store {
    struct __store_fields fields;
    struct __store_operations operations;
};

#endif