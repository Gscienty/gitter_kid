/*
    this module need setup pam-devel - centos OR libpam0g-dev
*/

#include "define.h"
#include "__pwio.h"

G_KID_EXTERN int user_create (const char* username);

G_KID_EXTERN struct db * user_get_users ();
G_KID_EXTERN int user_users_initialize_cursor (struct db *);
G_KID_EXTERN struct entry * user_users_get_cursor (struct db *);
G_KID_EXTERN int user_users_cursor_move_next (struct db *);
G_KID_EXTERN int user_users_cursor_move_prev (struct db *);