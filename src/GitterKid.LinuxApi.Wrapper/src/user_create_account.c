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

int create_account (const char *uname, const char *home, gid_t gid) {
    uid_t uid = __find_new_uid (uname);
    if (uid < 0) {
        return uid;
    }

    if (__create_home (home, uid, gid) == 0) {
        
    }
}