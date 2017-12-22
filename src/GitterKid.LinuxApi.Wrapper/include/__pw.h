#ifndef _G_KID_PW_
#define _G_KID_PW_
#include <stdio.h>
#include "__io.h"

void *__pw_dup (const void *);
void __pw_free (void *);
const char *__pw_getname (const void *);
void *__pw_parse (const char *);
int __pw_put (const void *, FILE *);

static struct ops pw_ops = {
    __pw_dup,
    __pw_free,
    __pw_getname,
    __pw_parse,
    __pw_put,
    fgets,
    fputs
};

#endif