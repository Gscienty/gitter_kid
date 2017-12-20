#include "user.h"

int main() {
    struct db *db = user_get_users ();


    printf ("%s\n", db->head->next->next->line);
    return 0;
}