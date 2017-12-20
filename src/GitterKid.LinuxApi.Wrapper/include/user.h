/*
    this module need setup pam-devel - centos OR libpam0g-dev
*/

#include "define.h"
#include "__pwio.h"

#ifndef _G_KID_USER_
#define _G_KID_USER_

G_KID_EXTERN int user_create (const char *);
G_KID_EXTERN int user_get_db_size ();
G_KID_EXTERN struct db * user_get_users ();
G_KID_EXTERN void user_dispose_users (struct db *);
G_KID_EXTERN int user_users_initialize_cursor (struct db *);
G_KID_EXTERN struct entry * user_users_get_cursor (struct db *);
G_KID_EXTERN int user_users_cursor_move_next (struct db *);
G_KID_EXTERN int user_users_cursor_move_prev (struct db *);

G_KID_EXTERN char * get_cursor_line (struct db *);
G_KID_EXTERN char * get_cursor_loginname (struct db *);
G_KID_EXTERN int get_cursor_userid (struct db *);
G_KID_EXTERN int get_cursor_groupid (struct db *);
G_KID_EXTERN char * get_cursor_username (struct db *);
G_KID_EXTERN char * get_cursor_dir (struct db *);
G_KID_EXTERN char * get_cursor_shell (struct db *);
#endif
