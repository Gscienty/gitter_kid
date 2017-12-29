#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "user.h"

int __find_new_uid (const char *username) {
    uid_t uid_min = 1000;
    uid_t uid_max = 60000;
    uid_t uid_use = uid_min;
    struct passwd *pwd;
    setpwent ();
    while ((pwd = getpwent ())) {
        if (strcmp (username, pwd->pw_name) == 0) {
            // username in use
            return -1;
        }

        if (pwd->pw_uid >= uid_use) {
            if (pwd->pw_uid > uid_max) {
                continue;
            }
            uid_use = pwd->pw_uid + 1;
        }
    }
    if (uid_use == uid_max + 1) {
        for (uid_use = uid_min; uid_use < uid_max; uid_use++) {
            if (!getpwuid (uid_use)) {
                break;
            }
        }
        if (uid_use == uid_max) {
            // user id in use
            return -2;
        }
    }

    return uid_use;
}

int __create_home (const char *home, uid_t uid, gid_t gid) {
    if (access (home, F_OK)) {
        if (mkdir (home, 0)) {
            // cannot create directory
            return -3;
        }
    }

    chown (home, uid, gid);
    chmod (home, 0777);

    return 0;
}

int create_account (const char *name, const char *home, gid_t gid) {
    pam_handle_t *pamh = NULL;
    int retval = PAM_SUCCESS;

    struct passwd *pampw = getpwuid (getuid ());
    if (pampw == NULL) {
        retval = PAM_USER_UNKNOWN;
    }

    struct pam_conv conv = { misc_conv, NULL };

    if (retval == PAM_SUCCESS) {
        retval = pam_start ("useradd", pampw->pw_name, &conv, &pamh);
    }
    if (retval == PAM_SUCCESS) {
        retval = pam_authenticate (pamh, 0);
        if (retval != PAM_SUCCESS) {
            pam_end (pamh, retval);
        }
    }
    if (retval == PAM_SUCCESS) {
        retval = pam_acct_mgmt (pamh, 0);
        if (retval != PAM_SUCCESS) {
            pam_end (pamh, retval);
        }
    }
    if (retval != PAM_SUCCESS) {
        DBG_LOG (DBG_ERROR, "PAM authentication failed");
        return -1;
    }

    if (getpwnam (name)) {
        DBG_LOG (DBG_ERROR, "use name is used");
        return -2;
    }

    uid_t uid = __find_new_uid (name);
    if (uid < 0) {
        pam_end (pamh, 0);
        DBG_LOG (DBG_ERROR, "could not find new uid");
        return -3;
    }
}