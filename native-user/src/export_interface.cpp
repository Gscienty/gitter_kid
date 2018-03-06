#include "export_interface.h"

GroupStore *gkid_user_get_group_store () {
    GroupStore *ret = new GroupStore ();
    ret->Initialize ();

    return ret;
}

void gkid_user_group_store_reload (GroupStore *store) {
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
}