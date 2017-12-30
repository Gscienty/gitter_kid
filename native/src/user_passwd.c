#include "user.h"
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

void *__pw_dup (const void *p) {
    if (p == NULL) {
        return NULL;
    }
    struct passwd *p_origin = (struct passwd *) p;
    struct passwd *p_ret = (struct passwd *) malloc (sizeof (*p_ret));

    if (p_ret == NULL) {
        return NULL;
    }
    *p_ret = *p_origin;
    if (!(p_ret->pw_name = strdup (p_origin->pw_name))) {
        return NULL;
    }
    if (!(p_ret->pw_passwd = strdup (p_origin->pw_passwd))) {
        return NULL;
    }
    if (!(p_ret->pw_gecos = strdup (p_origin->pw_gecos))) {
        return NULL;
    }
    if (!(p_ret->pw_dir = strdup (p_origin->pw_dir))) {
        return NULL;
    }
    if (!(p_ret->pw_shell = strdup (p_origin->pw_shell))) {
        return NULL;
    }

    return p_ret;
}

void __pw_free (void *p) {
    struct passwd *obj = (struct passwd *) p;
    free (obj->pw_dir);
    free (obj->pw_gecos);
    free (obj->pw_name);
    free (obj->pw_passwd);
    free (obj->pw_shell);
    free (obj);
}

const char *__pw_getname (const void *p) {
    const struct passwd *obj = (struct passwd *) p;
    return obj->pw_name;
}

#define PW_ITEM_COUNT 7
void *__pw_parse (const char *line) {
    int len = strlen (line);
    char *pwbuf = (char *) malloc (len + 1);
    if (!pwbuf) {
        // have not enough free memory
        return NULL;
    }
    strncpy (pwbuf, line, len);

    register char *cp;
    register int i;
    char *fields[PW_ITEM_COUNT];
    for (cp = pwbuf, i = 0; i < PW_ITEM_COUNT && cp; i++) {
        fields[i] = cp;
        while (*cp && *cp != ':') {
            cp++;
        }

        if (*cp) {
            *cp++ = 0;
        }
        else {
            cp = 0;
        }
    }

    if (i != PW_ITEM_COUNT || *fields[2] == 0 || *fields[3] == 0) {
        // illegal line
        free (pwbuf);
        return NULL;
    }

    struct passwd *ret = (struct passwd *) malloc (sizeof (*ret));
    if (!ret) {
        // have not enough free memory
        free (pwbuf);
        return NULL;
    }

    char *ep;
    ret->pw_name = strdup (fields[0]);
    ret->pw_passwd = strdup (fields[1]);
    if (fields[2][0] == 0 || ((ret->pw_uid = strtol (fields[2], &ep, 10)) == 0 && *ep)) {
        // could not transfer string to uid
        free (pwbuf);
        free (ret);
        return NULL;
    }
    if (fields[3][0] == 0 || ((ret->pw_gid = strtol (fields[3], &ep, 10)) == 0 && *ep)) {
        // could not transfer string to gid
        free (pwbuf);
        free (ret);
        return NULL;
    }
    ret->pw_gecos = strdup (fields[4]);
    ret->pw_dir = strdup (fields[5]);
    ret->pw_shell = strdup (fields[6]);

    free (pwbuf);

    return ret;
}

int __pw_put (const void *p, FILE *fp) {
    const struct passwd *pw = (const struct passwd *) p;

    return (putpwent (pw, fp) == -1) ? -1 : 0;
}


struct db *build_passwd_handle () {
    struct db *ret = (struct db *) malloc (sizeof (*ret));
    if (!ret) {
        return NULL;
    }

    strcpy(ret->filename, "/etc/passwd");
    ret->ops = &pw_ops;
    ret->head = ret->tail = ret->cursor = NULL;

    return ret;
}

int open_passwd (struct db *db) {
    db->locked = 1; // test lock
    return __open_db (db, O_RDWR);
}

void dispose_passwd (struct db *db) {
    __dispose_db (db);
}


int reset_passwd_cursor (struct db *db) {
    if (db == NULL) {
        // db not exist
        return -1;
    }
    if (!db->isopen) {
        // file /etc/passwd is not opend
        return -2;
    }

    db->cursor = db->head;
    return 0;
}

struct passwd *get_current_passwd (struct db *db) {
    if (db == NULL) {
        // db not exist
        return NULL;
    }
    if (!db->isopen) {
        // file /etc/passwd is not opend
        return NULL;
    }
    if (!db->cursor) {
        // nothing is pointed by cursor
        return NULL;
    }

    return (struct passwd *) db->cursor->ptr;
}

int move_passwd_cursor_next (struct db *db) {
    if (db == NULL) {
        // db not exist
        return -1;
    }
    if (!db->isopen) {
        // file /etc/passwd is not opend
        return -2;
    }
    if (!db->cursor) {
        // nothing is pointed by cursor
        return -3;
    }

    db->cursor = db->cursor->next;
    return db->cursor == NULL ? -4 /* next obj not exist */ : 0 /* successfully move to the next obj */;
}

char *get_passwd_username (struct passwd *pw) {
    return pw->pw_name;
}
char *get_passwd_dir (struct passwd *pw) {
    return pw->pw_dir;
}
char *get_passwd_gecos (struct passwd *pw) {
    return pw->pw_gecos;
}
char *get_passwd_passwd (struct passwd *pw) {
    return pw->pw_passwd;
}
char *get_passwd_shell (struct passwd *pw) {
    return pw->pw_shell;
}
int get_passwd_uid (struct passwd *pw) {
    return pw->pw_uid;
}
int get_passwd_gid (struct passwd *pw) {
    return pw->pw_gid;
}

int __append_passwd (struct db *db, struct passwd *pw) {

}