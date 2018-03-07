#ifndef _G_KID_USER_EXPORT_INTERFACE_
#define _G_KID_USER_EXPORT_INTERFACE_
#include "group.h"
#include "passwd.h"
#include "shadow.h"

struct group_item_eachor {
    std::vector<GroupItem>::iterator begin;
    std::vector<GroupItem>::iterator iter;
    std::vector<GroupItem>::iterator end;
};

GroupStore *gkid_user_get_group_store ();
void gkid_user_group_store_reload (GroupStore *);
group_item_eachor *gkid_user_group_collection_generate_eachor (GroupStore *);
bool gkid_user_group_eachor_hasnext (group_item_eachor *);
void gkid_user_group_eachor_next (group_item_eachor *);

const char *gkid_user_group_eachor_current_name (group_item_eachor *);
const char *gkid_user_group_eachor_current_passwd (group_item_eachor *);
uint32_t gkid_user_group_eachor_current_gid (group_item_eachor *);

#endif