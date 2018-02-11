#include "passwd.h"

int main () {
    struct __passwd_collection *collection = (struct __passwd_collection *) passwd_get_collection ();

    passwd_save_collection (collection);
}