/*
    this module need setup pam-devel - centos OR libpam0g-dev
*/
#ifndef _G_KID_USER_
#define _G_KID_USER_


#include "define.h"
#include "__pw.h"

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
#endif
