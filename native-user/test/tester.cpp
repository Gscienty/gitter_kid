#include "passwd.h"
#include <algorithm>
#include <iostream>

int main () {

    PasswdStore store;

    std::vector <PasswdItem> items = store.Get ();

    std::for_each (items.begin (), items.end (), [] (const PasswdItem& item) -> void {
        std::cout << item.Serialize () << std::endl;
    });

    items.pop_back ();

    store.Put (items);

    return 0;
}