#include "passwd.h"
#include "group.h"
#include "shadow.h"
#include "export_interface.h"
#include <algorithm>
#include <iostream>
#include <crypt.h>

void test_group () {
    GroupStore *store = gkid_user_get_group_store ();
    group_item_eachor *eachor = gkid_user_group_collection_generate_eachor (store);

    while (gkid_user_group_eachor_hasnext (eachor)) {
        gkid_user_group_eachor_next (eachor);
        std::cout << gkid_user_group_eachor_current_name(eachor) << std::endl;
        std::cout << gkid_user_group_eachor_current_passwd(eachor) << std::endl;
        std::cout << gkid_user_group_eachor_current_gid(eachor) << std::endl;

        group_item_user_eachor *users = gkid_user_group_item_users_generate_eachor (eachor);

        while (gkid_user_group_users_eachor_hasnext (users)) {
            gkid_user_group_users_eachor_next (users);

            std::cout << gkid_user_group_users_eachor_current_name (users) << std::endl;
        }
    }
}

void test_passwd () {
    PasswdStore *store = gkid_user_get_passwd_store ();
    passwd_item_eachor *eachor = gkid_user_passwd_collection_generate_eachor (store);

    while (gkid_user_passwd_eachor_hasnext (eachor)) {
        gkid_user_passwd_eachor_next (eachor);

        std::cout << gkid_user_passwd_eachor_current_username (eachor) << std::endl;
        std::cout << gkid_user_passwd_eachor_current_home (eachor) << std::endl;
        std::cout << gkid_user_passwd_eachor_current_gecos (eachor) << std::endl;
        std::cout << gkid_user_passwd_eachor_current_passwd (eachor) << std::endl;
        std::cout << gkid_user_passwd_eachor_current_shell (eachor) << std::endl;
        std::cout << gkid_user_passwd_eachor_current_gid (eachor) << std::endl;
        std::cout << gkid_user_passwd_eachor_current_uid (eachor) << std::endl;
    }
}

int main () {
    test_passwd ();
    return 0;
}