#include "passwd.h"

int main () {
    __passwd_open_store ();
    struct __passwd_collection *collection = (struct __passwd_collection *) __passwd_store_read_data ();

    struct __passwd_item *item = collection->head;

    for (; item != NULL; item = item->next) {
        puts (item->record.username);
    }
    return 0;
}