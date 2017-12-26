#ifndef _G_KID_SPW_
#define _G_KID_SPW_
#include <stdio.h>
#include "__io.h"

void *__spw_dup (const void *);
void __spw_free (void *);
const char *__spw_getname (const void *);
void *__spw_parse (const char *);
int __spw_put (const void *, FILE *);

static struct ops spw_ops = {
    __spw_dup,
    __spw_free,
    __spw_getname,
    __spw_parse,
    __spw_put,
    fgets,
    fputs
};

#endif