#include "passwd.h"
#include "group.h"
#include "shadow.h"
#include "export_interface.h"
#include <algorithm>
#include <iostream>
#include <crypt.h>

int main () {
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
    return 0;
}