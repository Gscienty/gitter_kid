#include "passwd.h"
#include "shadow.h"
#include <algorithm>
#include <iostream>

int main () {

    ShadowItem item = "games:*:17379:0:99999:7:::";
    std::cout << item.Serialize () << std::endl;
    return 0;
}