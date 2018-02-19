#include "passwd.h"
#include "group.h"
#include "shadow.h"
#include <algorithm>
#include <iostream>
#include <crypt.h>

int main () {
    ShadowPasswd passwd = "$6$Of09ZmUC$3XyIIhCjoss52No.RI9lKPvySaqzFiZJcNvcA7ZyqMfwDW2GH8IBKHgMuBxw5T7jnRmo9fhOuXnftVn5vPkXB1";

    std::cout << (passwd == std::string ("ant")) << std::endl;
    return 0;
}