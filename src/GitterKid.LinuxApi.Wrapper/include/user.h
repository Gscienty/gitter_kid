/*
    this module need setup pam-devel - centos OR libpam0g-dev
*/
#ifndef _G_KID_USER_
#define _G_KID_USER_


#include "define.h"
#include "__pw.h"

struct db *build_passwd_handle ();
int open_passwd (struct db *);

int reset_passwd_cursor (struct db *);
struct passwd *get_current_passwd (struct db *);
int move_passwd_cursor_next (struct db *);

char *get_passwd_username (struct passwd *);
char *get_passwd_dir (struct passwd *);
char *get_passwd_gecos (struct passwd *);
int get_passwd_uid (struct passwd *);
int get_passwd_gid (struct passwd *);
char *get_passwd_passwd (struct passwd *);
char *get_passwd_shell (struct passwd *);
#endif
