#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include "user.h"

int __find_new_uid() {
    const struct passwd* pwd;

    uid_t user_id = 1000;

    
}

/*
** create a new user.
** if current user unknow then return -1
** if current authenticate failure then return -2
** if username exist then return -3
*/
int user_create (const char* username) {
    struct struct pam_conv conv = {
        misc_conv,
        NULL
    };
    int retval;

    // get current user passwd
    struct passwd* pam_passwd = getpwuid (getuid ());

    if ( pam_passwd == NULL ) {
        // pam_user_unknow
        return -1;
    }
    
    // pam start
    pam_handle_t pam_h;
    retval = pam_start ("useradd", pam_passwd->pw_name, &conv, &pam_h);

    // authenticate
    if ( retval == PAM_SUCCESS ) {
        retval = pam_authenticate (pam_h, 0);

        if ( retval != PAM_SUCCESS ) {
            pam_end (pam_h, retval);
            return -2;
        }
    }
    // account management
    if ( retval == PAM_SUCCESS ) {
        retval = pam_acct_mgmt (pamh, 0);

        if ( retval != PAM_SUCCESS ) {
            pam_end (pam_h, retval);
            return -3;
        }
    }

    // check this username is exist.
    if (getpwnam(username)) {
        pam_end(pam_h, retval);
        return -3;
    }

    int uid = __find_new_uid();
}

struct db * user_get_users () {
    struct db *db_instance = (struct db *) malloc (sizeof (*db_instance));
    pw_open(db_instance, O_REONLY);
    return db_instance;
}

void user_users_initialize_cursor (struct db *db) {
    if ( db == NULL ) {
        return -1;
    }
    db->cursor = db->head;
    return 0;
}

struct entry * user_users_get_cursor (struct db *db) {
    if ( db == NULL ) {
        return NULL;
    }
    if ( db->cursor == NULL ) {
        return NULL;
    }
    return db->cursor;
}

int user_users_cursor_move_next (struct db *db) {
    if ( db == NULL ) {
        return -1;
    }
    if ( db->cursor == NULL ) {
        return -2;
    }
    db->cursor = db->cursor->next;
    return 0;
}

int user_users_cursor_move_prev (struct db *db) {
    if ( db == NULL ) {
        return -1;
    }
    if ( db->cursor == NULL ) {
        return -2;
    }
    db->cursor = db->cursor->prev;
    return 0;
}

char * get_cursor_line (struct db *db) {
    struct entry *entry_instance = get_cursor(db);
    if (entry == NULL) {
        return NULL;
    }
    return entry_instance->line;
}