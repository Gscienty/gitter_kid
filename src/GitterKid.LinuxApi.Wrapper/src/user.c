#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <pwd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "user.h"
#include "__io.h"

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
    return open_db (db, O_RDONLY);
}

void dispose_passwd (struct db *db) {
    dispose_db (db);
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

struct db *build_group_handle () {
    struct db *ret = (struct db *) malloc (sizeof (*ret));
    if (!ret) {
        return NULL;
    }

    strcpy(ret->filename, "/etc/group");
    ret->ops = &grp_ops;
    ret->head = ret->tail = ret->cursor = NULL;
}

int open_group (struct db *db) {
    return open_db (db, O_RDONLY);
}

void dispose_group (struct db *db) {
    dispose_db (db);
}

int reset_group_cursor (struct db *db) {
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

struct group *get_current_group (struct db *db) {
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

    return (struct group *) db->cursor->ptr;
}

int move_group_cursor_next (struct db *db) {
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

char *get_group_name (struct group *grp) {
    return grp->gr_name;
}
char *get_group_passwd (struct group *grp) {
    return grp->gr_passwd;
}
int get_group_gid (struct group *grp) {
    return grp->gr_gid;
}
char** get_group_member_cursor (struct group *grp) {
    return grp->gr_mem;
}
size_t get_member_cursor_size () {
    return sizeof (char **);
}