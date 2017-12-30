
#ifndef _G_KID_USER_
#define _G_KID_USER_

#include "define.h"
#include <stdio.h>
#include <grp.h>

struct entry {
    char *line; // store /etc/passwd file's line
    void *ptr;  // store struct passwd
    struct entry *prev;
    struct entry *next;
    int changed : 1;
};

struct ops {
    // make a copy of the object
    // and all strings pointed by it, in malloced memory.
    void *(*dup) (const void *);

    // free the struct passwd
    void (*free) (void *);

    // return the name of the object.
    const char *(*getname) (const void *);

    // parse a string, return object.
    void *(*parse) (const char *);

    // write the object to the file.
    int (*put) (const void *, FILE *);

    char *(*fgets) (char *, int, FILE *);
    int (*fputs) (const char *, FILE *);
};

struct db {
    // name of data file
    char filename[1024];

    struct ops *ops;

    FILE *fp;

    struct entry *head;
    struct entry *tail;
    struct entry *cursor;

    int changed : 1;
    int isopen : 1;
    int locked : 1;
    int readonly : 1;
};

// open file & tranfer to struct db
int __open_db (struct db *, int);
int __save_db (struct db *);
int __append_db (struct db *, const void *);
void __dispose_db (struct db *);

void *__pw_dup (const void *);
void __pw_free (void *);
const char *__pw_getname (const void *);
void *__pw_parse (const char *);
int __pw_put (const void *, FILE *);

static struct ops pw_ops = {
    __pw_dup,
    __pw_free,
    __pw_getname,
    __pw_parse,
    __pw_put,
    fgets,
    fputs
};

void *__spw_dup (const void *);
void __spw_free (void *);
const char *__spw_getname (const void *);
void *__spw_parse (const char *);
int __spw_put (const void *, FILE *);

static struct ops spw_ops = {
    __spw_dup,
    __spw_free,
    __spw_getname,
    __spw_parse,
    __spw_put,
    fgets,
    fputs
};

void *__grp_dup (const void *);
void __grp_free (void *);
const char *__grp_getname (const void *);
void *__grp_parse (const char *);
int __grp_put (const void *, FILE *);

static struct ops grp_ops = {
    __grp_dup,
    __grp_free,
    __grp_getname,
    __grp_parse,
    __grp_put,
    fgets,
    fputs
};


G_KID_EXTERN struct db *build_passwd_handle ();
G_KID_EXTERN int open_passwd (struct db *);
G_KID_EXTERN void dispose_passwd (struct db *);

G_KID_EXTERN int reset_passwd_cursor (struct db *);
G_KID_EXTERN struct passwd *get_current_passwd (struct db *);
G_KID_EXTERN int move_passwd_cursor_next (struct db *);

G_KID_EXTERN char *get_passwd_username (struct passwd *);
G_KID_EXTERN char *get_passwd_dir (struct passwd *);
G_KID_EXTERN char *get_passwd_gecos (struct passwd *);
G_KID_EXTERN int get_passwd_uid (struct passwd *);
G_KID_EXTERN int get_passwd_gid (struct passwd *);
G_KID_EXTERN char *get_passwd_passwd (struct passwd *);
G_KID_EXTERN char *get_passwd_shell (struct passwd *);


G_KID_EXTERN struct db *build_group_handle ();
G_KID_EXTERN int open_group (struct db *);
G_KID_EXTERN void dispose_group (struct db *);

G_KID_EXTERN int reset_group_cursor (struct db *);
G_KID_EXTERN struct group *get_current_group (struct db *);
G_KID_EXTERN int move_group_cursor_next (struct db *);

G_KID_EXTERN char *get_group_name (struct group *);
G_KID_EXTERN char *get_group_passwd (struct group *);
G_KID_EXTERN int get_group_gid (struct group *);

struct group_member {
    char** base;
    int cursor;
    int count;
};
G_KID_EXTERN struct group_member *get_group_member_cursor (struct group *);
G_KID_EXTERN void reset_group_member_cursor (struct group_member *);
G_KID_EXTERN int get_group_member_count (struct group_member *);
G_KID_EXTERN char *get_current_group_member_name (struct group_member *);
G_KID_EXTERN int group_member_move_next (struct group_member *);
G_KID_EXTERN int dispose_group_member (struct group_member *);

G_KID_EXTERN struct db *build_shadow_handle ();
G_KID_EXTERN int open_shadow (struct db *);
G_KID_EXTERN void dispose_shadow (struct db *);

G_KID_EXTERN int create_account (struct db *db, const char *name, const char *home, const char *shell, gid_t gid);

#endif
