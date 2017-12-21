#include "__pw.h"
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void *__pw_dup (const void *p) {
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
    ret->pw_name = fields[0];
    ret->pw_passwd = fields[1];
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
    ret->pw_gecos = fields[4];
    ret->pw_dir = fields[5];
    ret->pw_shell = fields[6];

    return ret;
}

int __pw_put (const void *p, FILE *fp) {
    const struct passwd *pw = (const struct passwd *) p;

    return (putpwent (pw, fp) == -1) ? -1 : 0;
}