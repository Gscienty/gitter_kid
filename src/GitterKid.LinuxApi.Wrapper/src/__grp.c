#include <grp.h>
#include <string.h>
#include <stdlib.h>
#include "__grp.h"

void *__grp_dup (const void *p) {
    if (p == NULL) {
        return NULL;
    }
    struct group *p_origin = (struct group *) p;
    struct group *p_ret = (struct group *) malloc (sizeof (*ret));
    if (p_ret == NULL) {
        // have not enough free memory
        return NULL;
    }

    *p_ret = *p_origin;
    if (!(p_ret->gr_name = strdup (p_origin->gr_name))) {
        return NULL;
    }
    if (!(p_ret->gr_passwd = strdup (p_origin->gr_passwd))) {
        return NULL;
    }

    int member_count;
    for (member_count = 0; p_origin->gr_mem[member_count]; member_count++);
    p_ret->gr_mem = (char **) malloc (sizeof (char *) * (member_count + 1));
    if (!p_ret->gr_mem) {
        return NULL;
    }
    int i = 0;
    while (p_origin->gr_mem[i]) {
        p_ret->gr_mem[i] = strdup (p_origin->gr_mem[i]);
        i++;
    }
    p_ret->gr_mem[i] = NULL;

    return p_ret;
}

void __grp_free (void *p) {
    if (p == NULL) {
        return ;
    }
    struct group *grp = (struct group *) p;
    free (grp->gr_name);
    free (grp->gr_passwd);
    char **mem = grp->gr_mem;
    while (*mem) { free (*mem++); }
    free (gr->gr_mem);
}

const char *__grp_getname (const void *p) {
    if (p == NULL) {
        return NULL;
    }
    struct group *grp = (struct group *) p;
    return grp->gr_name;
}

#define GRP_ITEM_COUNT 4
void *__grp_parse (const char *line) {
    int len = strlen (line);
    char *grpbuf = (char *) malloc (sizeof (len + 1));
    if (grpbuf == NULL) {
        // have not enough free memory
        return NULL;
    }
    strncpy (grpbuf, line, len);

    register char *cp;
    register int i;
    char *fields[GRP_ITEM_COUNT];
    for (cp = grpbuf, i = 0; i < GRP_ITEM_COUNT && cp; i++) {
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

    if (i != GRP_ITEM_COUNT) {
        free (grpbuf);
        return NULL;
    }

    struct group *grp = (struct group *) malloc (sizeof (*grp));
    grp->gr_name = strdup (fields[0]);
    grp->gr_passwd = strdup (fields[1]);
    grp->gr_gid = strtol (fields[2]);
    
}

