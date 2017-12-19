#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include "user.h"

int __find_new_uid() {
    
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