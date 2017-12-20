#include "user.h"

int main() {
    struct db *db = user_get_users ();
    
    user_users_initialize_cursor (db);

    do {
        printf ("%d, %s, %d, %s, %s, %s\n",
            get_cursor_userid (db),
            get_cursor_loginname (db),
            get_cursor_groupid (db),
            get_cursor_username (db),
            get_cursor_dir (db),
            get_cursor_shell (db)
        );
    } while (user_users_cursor_move_next (db) == 0);
    return 0;
}