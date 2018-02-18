#include "passwd.h"
#include "group.h"
#include "shadow.h"
#include <algorithm>
#include <iostream>

int main () {

    GroupStore store;
    store.Initialize ();
    auto items = store.Get ();

    std::for_each (items.begin (), items.end (), [] (auto item) -> void {
        std::cout << item.Serialize () << std::endl;
    });

    return 0;
}