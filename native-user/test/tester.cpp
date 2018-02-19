#include "passwd.h"
#include "group.h"
#include "shadow.h"
#include <algorithm>
#include <iostream>
#include <crypt.h>

int main () {
    ShadowPasswd passwd = "aaaaaa";

    std::cout << (passwd == "aaaaaaa") << (passwd == "aaaaaa") << std::endl;

    return 0;
}