#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <pwd.h>
#include "user.h"

int user_create(const char* username) {
    struct struct pam_conv conv = {
        misc_conv,
        NULL
    };
    struct passwd* pam_passwd;

    pam_handle_t pam_h = NULL;

    if ( pam_passwd == NULL ) {
        return -1;
    }
}