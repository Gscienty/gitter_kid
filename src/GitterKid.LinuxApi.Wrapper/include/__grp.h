#ifndef _G_KID_GRP_
#define _G_KID_GRP_
#include <stdio.h>
#include "__io.h"

void *__grp_dup (const void *);
void __grp_free (void *);
const char *__grp_getname (const void *);
void *__grp_parse (const char *);
int __grp_put (const void *, FILE *);

static struct ops pw_ops = {
    __grp_dup,
    __grp_free,
    __grp_getname,
    __grp_parse,
    __grp_put,
    fgets,
    fputs
};

#endif