#ifndef _G_KID_USER_DEFINE_
#define _G_KID_USER_DEFINE_

#ifndef NULL
#define NULL 0
#endif

#include <stdio.h>

#if __GNUC__ >= 4
#define G_KID_USER_EXTERN __attribute__((visibility("default")))
#else
#define G_KID_USER_EXTERN
#endif // __GNUC__

#endif
