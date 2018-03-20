#include "export_interface.h"

PasswdStore *gkid_user_get_passwd_store () {
    PasswdStore *ret = new PasswdStore ();
    ret->Initialize ();

    return ret;
}

void gkid_user_passwd_store_reload (PasswdStore *store) {
    if (store == NULL) {
        return;
    }
    store->Reloading ();
}

passwd_item_eachor *gkid_user_passwd_collection_generate_eachor (PasswdStore *store) {
    if (store == NULL) {
        return NULL;
    }
    std::vector<PasswdItem> &items = store->Get ();
    passwd_item_eachor *eachor = new passwd_item_eachor ();

    eachor->begin = items.begin ();
    eachor->end = items.end ();
    eachor->iter = items.begin ();
    eachor->first_flag = true;

    return eachor;
}

void gkid_user_passwd_collection_dtor (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return;
    }

    delete eachor;
}

bool gkid_user_passwd_eachor_hasnext (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return false;
    }
    return eachor->iter != eachor->end && eachor->iter + 1 != eachor->end;
}

void gkid_user_passwd_eachor_next (passwd_item_eachor *eachor) {
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

void gkid_user_passwd_eachor_reset (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return;
    }
    eachor->first_flag = true;
    eachor->iter = eachor->begin;
}

const char *gkid_user_passwd_eachor_current_username (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return NULL;
    }
    return eachor->iter->GetUserName ().c_str ();
}

const char *gkid_user_passwd_eachor_current_home (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return NULL;
    }
    return eachor->iter->GetHome ().c_str ();
}

const char *gkid_user_passwd_eachor_current_gecos (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return NULL;
    }
    return eachor->iter->GetGecos ().c_str ();
}

const char *gkid_user_passwd_eachor_current_passwd (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return NULL;
    }
    return eachor->iter->GetPasswd ().c_str ();
}

const char *gkid_user_passwd_eachor_current_shell (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return NULL;
    }
    return eachor->iter->GetShell ().c_str ();
}

const char *gkid_user_passwd_eachor_current_gid (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return NULL;
    }
    return eachor->iter->GetGID ();
}

const char *gkid_user_passwd_eachor_current_uid (passwd_item_eachor *eachor) {
    if (eachor == NULL) {
        return NULL;
    }
    return eachor->iter->GetUID ();
}
