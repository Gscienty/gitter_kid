#ifndef _G_KID_DEFINE_

#if __GNUC__ >= 4
#define G_KID_EXTERN __attribute__((visibility("default")))
#else
#define G_KID_EXTERN
#endif // __GNUC__

#include "log.h"

#endif //_G_KID_DEFINE_