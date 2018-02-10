#ifndef _G_KID_USER_PASSWD_
#define _G_KID_USER_PASSWD_
#include "define.h"
#include "store.h"

int __passwd_open_store ();
void *__passwd_store_read_data ();
void __passwd_store_data_dtor (void *data);

struct __passwd_record {
    char *username;
    char *password;
    unsigned int user_id;
    unsigned int group_id;
    char *description;
    char *home;
    char *shell;
};

struct __passwd_item {
    struct __passwd_record record;
    unsigned char modify_flag;
    
    struct __passwd_item *prev;
    struct __passwd_item *next;
};

struct __passwd_collection {
    struct __passwd_item *head;
    struct __passwd_item *tail;
    struct __passwd_item *cursor;
};

static struct __store __passwd_store = {
    {
        "./test/etc/passwd",
        0
    },
    {
        __passwd_open_store,
        __passwd_store_read_data,
        __passwd_store_data_dtor
    }
};

G_KID_USER_EXTERN struct __passwd_collection *passwd_get_collection ();
G_KID_USER_EXTERN void passwd_collection_reset (struct __passwd_collection *collection);
G_KID_USER_EXTERN int passwd_collection_has_next (struct __passwd_collection *collection);
G_KID_USER_EXTERN struct __passwd_record *passwd_collection_next (struct __passwd_collection *collection);

#endif