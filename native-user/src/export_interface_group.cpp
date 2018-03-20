#include "export_interface.h"

GroupStore *gkid_user_get_group_store () {
    GroupStore *ret = new GroupStore ();
    ret->Initialize ();

    return ret;
}

void gkid_user_group_store_reload (GroupStore *store) {
    if (store == NULL) {
        return;
    }
    store->Reloading ();
}


group_item_eachor *gkid_user_group_collection_generate_eachor (GroupStore *store) {
    if (store == NULL) {
        return NULL;
    }
    std::vector<GroupItem> &items = store->Get ();
    group_item_eachor *eachor = new group_item_eachor ();

    eachor->begin = items.begin ();
    eachor->end = items.end ();
    eachor->iter = items.begin ();
    eachor->first_flag = true;

    return eachor;
}

void gkid_user_group_collection_dtor (group_item_eachor *eachor) {
    if (eachor == NULL) {
        return;
    }
    delete eachor;
}

bool gkid_user_group_eachor_hasnext (group_item_eachor *eachor) {
    if (eachor == NULL) {
        return false;
    }
    return eachor->iter != eachor->end && eachor->iter + 1 != eachor->end;
}
void gkid_user_group_eachor_next (group_item_eachor *eachor) {
    if (eachor == NULL) {
        return;
    }
    if (eachor->first_flag) {
        eachor->iter = eachor->begin;
        eachor->first_flag = false;
        return;
    }
    eachor->iter++;
}
const char *gkid_user_group_eachor_current_name (group_item_eachor *eachor) { return eachor->iter->GetName ().c_str (); }
const char *gkid_user_group_eachor_current_passwd (group_item_eachor *eachor) { return eachor->iter->GetPasswd ().c_str (); }
uint32_t gkid_user_group_eachor_current_gid (group_item_eachor *eachor) { return eachor->iter->GetGID (); }
void gkid_user_group_eachor_reset (group_item_eachor *eachor) {
    eachor->first_flag = true;
    eachor->iter = eachor->begin;
}

group_item_user_eachor *gkid_user_group_item_users_generate_eachor (group_item_eachor *item_eachor) {
    if (item_eachor == NULL) {
        return NULL;
    }
    std::vector<std::string> &users = item_eachor->iter->GetUsers ();
    group_item_user_eachor *users_eachor = new group_item_user_eachor ();

    users_eachor->begin = users.begin ();
    users_eachor->end = users.end ();
    users_eachor->iter = users.begin ();
    users_eachor->first_flag = true;

    return users_eachor;
}

void gkid_user_group_item_users_dtor (group_item_eachor *eachor) {
    if (eachor == NULL) {
        return;
    }

    delete eachor;
}

bool gkid_user_group_users_eachor_hasnext (group_item_user_eachor *eachor) { return eachor->iter != eachor->end && eachor->iter + 1 != eachor->end; }
void gkid_user_group_users_eachor_next (group_item_user_eachor *eachor) { 
    if (eachor->first_flag) {
        eachor->iter = eachor->begin;
        eachor->first_flag = false;
        return;
    }
    eachor->iter++;
}
const char *gkid_user_group_users_eachor_current_name (group_item_user_eachor *eachor) { return eachor->iter->c_str (); }

void gkid_user_group_users_eachor_reset (group_item_user_eachor *eachor) {
    if (eachor == NULL) {
        return;
    }
    eachor->first_flag = true;
    eachor->iter = eachor->begin;
}