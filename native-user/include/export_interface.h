#ifndef _G_KID_USER_EXPORT_INTERFACE_
#define _G_KID_USER_EXPORT_INTERFACE_
#include "group.h"
#include "passwd.h"
#include "shadow.h"

struct group_item_eachor {
    bool first_flag;
    std::vector<GroupItem>::iterator begin;
    std::vector<GroupItem>::iterator iter;
    std::vector<GroupItem>::iterator end;
};

struct group_item_user_eachor {
    bool first_flag;
    std::vector<std::string>::iterator begin;
    std::vector<std::string>::iterator iter;
    std::vector<std::string>::iterator end;
};

struct passwd_item_eachor {
    bool first_flag;
    std::vector<PasswdItem>::iterator begin;
    std::vector<PasswdItem>::iterator iter;
    std::vector<PasswdItem>::iterator end;
};

struct shadow_item_eachor {
    bool first_flag;
};

GroupStore *gkid_user_get_group_store ();
void gkid_user_group_store_reload (GroupStore *);
group_item_eachor *gkid_user_group_collection_generate_eachor (GroupStore *);
void gkid_user_group_collection_dtor (group_item_eachor *);
bool gkid_user_group_eachor_hasnext (group_item_eachor *);
void gkid_user_group_eachor_next (group_item_eachor *);
void gkid_user_group_eachor_reset (group_item_eachor *);

const char *gkid_user_group_eachor_current_name (group_item_eachor *);
const char *gkid_user_group_eachor_current_passwd (group_item_eachor *);
uint32_t gkid_user_group_eachor_current_gid (group_item_eachor *);

group_item_user_eachor *gkid_user_group_item_users_generate_eachor (group_item_eachor *);
void gkid_user_group_item_users_dtor (group_item_eachor *);
bool gkid_user_group_users_eachor_hasnext (group_item_user_eachor *);
void gkid_user_group_users_eachor_next (group_item_user_eachor *);
void gkid_user_group_users_eachor_reset (group_item_user_eachor *);

const char *gkid_user_group_users_eachor_current_name (group_item_user_eachor *);

PasswdStore *gkid_user_get_passwd_store ();
void gkid_user_passwd_store_reload (PasswdStore *);
passwd_item_eachor *gkid_user_passwd_collection_generate_eachor (PasswdStore *);
void gkid_user_passwd_collection_dtor (passwd_item_eachor *);
bool gkid_user_passwd_eachor_hasnext (passwd_item_eachor *);
void gkid_user_passwd_eachor_next (passwd_item_eachor *);
void gkid_user_passwd_eachor_reset (passwd_item_eachor *);

const char *gkid_user_passwd_eachor_current_username (passwd_item_eachor *);
const char *gkid_user_passwd_eachor_current_home (passwd_item_eachor *);
const char *gkid_user_passwd_eachor_current_gecos (passwd_item_eachor *);
const char *gkid_user_passwd_eachor_current_passwd (passwd_item_eachor *);
const char *gkid_user_passwd_eachor_current_shell (passwd_item_eachor *);
unsigned int gkid_user_passwd_eachor_current_gid (passwd_item_eachor *);
unsigned int gkid_user_passwd_eachor_current_uid (passwd_item_eachor *);

#endif