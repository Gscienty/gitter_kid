/*
    this module need setup pam-devel - centos OR libpam0g-dev
*/
#ifndef _G_KID_USER_
#define _G_KID_USER_


#include "define.h"
#include "__pw.h"
#include "__grp.h"

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
};
G_KID_EXTERN struct group_member *get_group_member_cursor (struct group *);
G_KID_EXTERN void reset_group_member_cursor (struct group_member *);
G_KID_EXTERN char *get_current_group_member_name (struct group_member *);
G_KID_EXTERN int group_member_move_next (struct group_member *);
G_KID_EXTERN int dispose_group_member (struct group_member *);
#endif
