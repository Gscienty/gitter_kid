#include "export_interface.h"

ShadowItem *gkid_user_get_shadow_store () {
    ShadowItem *ret = new ShadowItem ();
    ret->Initialize ();
    return ret;
}

void gkid_user_shadow_store_reload (ShadowStore *store) {
    if (store == NULL) {
        return;
    }
    store->Reloading ();
}

shadow_item_eachor *gkid_user_shadow_collection_generate_eachor (ShadowStore *store) {
    if (store == NULL) {
        return;
    }
    std::vector<ShadowItem> &items = store->Get ();
    shadow_item_eachor *eachor = new shadow_item_eachor ();

    eachor->begin = items.begin ();
    eachor->iter = items.begin ();
    eachor->end = items.end ();
    eachor->first_flag = true;

    return eachor;
}

void gkid_user_shadow_collection_dtor (shadow_item_eachor *eachor) {
    if (eachor == NULL) {
        return;
    }
    delete eachor;
}

bool gkid_user_shadow_eachor_hasnext (shadow_item_eachor *eachor) {
    if (eachor == NULL) {
        return false;
    }
    return eachor->iter != eachor->end && eachor->iter + 1 != eachor->end;
}

void gkid_user_shadow_eachor_next (shadow_item_eachor *eachor) {
    if (eachor == NULL) {
        return false;
    }
    if (eachor->first_flag) {
        eachor->iter = eachor->begin;
        eachor->first_flag = false;
        return;
    }
    eachor->iter++;
}

void gkid_user_shadow_eachor_reset (shadow_item_eachor *eachor) {
    if (eachor == NULL) {
        return;
    }
    eachor->first_flag = true;
    eachor->iter = eachor->begin;
}

const char *gkid_user_shadow_eachor_current_username (shadow_item_eachor *eachor) {
    if (eachor == NULL) {
        return NULL;
    }
    return eachor->iter->GetUserName ().c_str ();
}