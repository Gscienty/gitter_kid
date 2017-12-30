#include "user.h"
#include <shadow.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>

void *__spw_dup (const void *p) {
    struct spwd *spw = (struct spwd *) p;
    struct spwd *ret = (struct spwd *) malloc (sizeof (*ret));
    if (ret == NULL) {
        return NULL;
    }
    *ret = *spw;

    ret->sp_namp = strdup (spw->sp_namp);
    if (ret->sp_namp == NULL) {
        return NULL;
    }
    ret->sp_pwdp = strdup (spw->sp_pwdp);
    if (ret->sp_pwdp == NULL) {
        return NULL;
    }

    return ret;
}

void __spw_free (void *p) {
    struct spwd *spw = (struct spwd *) p;
    if (spw == NULL) {
        return ;
    }

    free (spw->sp_namp);
    free (spw->sp_pwdp);
    free (spw);
}

const char *__spw_getname (const void *p) {
    struct spwd *spw = (struct spwd *) p;
    if (spw == NULL) {
        return NULL;
    }
    
    return spw->sp_namp;
}

void *__spw_parse (const char *line) {
    return (void *) sgetspent (line);
}

int __spw_put (const void *ent, FILE *file) {
    const struct spwd *spw = (const struct spwd *) ent;

    return (putspent (spw, file) == -1) ? -1 : 0;
}

struct db *build_shadow_handle () {
    struct db *ret = (struct db *) malloc (sizeof (*ret));
    if (!ret) {
        return NULL;
    }

    strcpy(ret->filename, "/etc/shadow");
    ret->ops = &spw_ops;
    ret->head = ret->tail = ret->cursor = NULL;
}

int open_shadow (struct db *db) {
    return __open_db (db, O_RDONLY);
}

void dispose_shadow (struct db *db) {
    __dispose_db (db);
}