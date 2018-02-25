#include "passwd.h"
#include "group.h"
#include "shadow.h"
#include <algorithm>
#include <iostream>
#include <crypt.h>

int main () {
    GroupStore store;
    store.Initialize ();

    std::vector<GroupItem> groups = store.Get();

    std::for_each (groups.begin (), groups.end (), [] (const GroupItem &item) -> void {
        std::cout << item.Serialize () << std::endl;
    });

    return 0;
}