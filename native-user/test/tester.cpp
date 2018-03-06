#include "passwd.h"
#include "group.h"
#include "shadow.h"
#include "export_interface.h"
#include <algorithm>
#include <iostream>
#include <crypt.h>

int main () {

    GroupStore *store = gkid_user_get_group_store ();

    std::vector<GroupItem> *items = gkid_user_group_store_get_collection (store);
    return 0;
}