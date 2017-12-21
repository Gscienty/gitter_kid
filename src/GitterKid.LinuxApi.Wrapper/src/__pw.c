#include "__pw.h"
#include <pwd.h>
#include <unistd.h>
#include <string.h>

void *__pw_dup (const void *p) {
    struct passwd *p_origin = (struct passwd *) p;
    struct passwd *p_ret = (struct passwd *) malloc (sizeof (*p_ret));

    if (p_ret == NULL) {
        return NULL;
    }
    if (!(p_ret->pw_name = ))
}