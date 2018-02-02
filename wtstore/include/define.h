#ifndef _WTSTORE_DEFINE_

#if __GNUC__ >= 4
#define WTSTORE_EXTERN __attribute__((visibility("default")))
#else
#define WTSTORE_EXTERN
#endif // __GNUC__

#include "log.h"

#endif //_WTSTORE_DEFINE_