#ifndef _WTSTORE_DATABASE_
#define _WTSTORE_DATABASE_

#include "define.h"
#include <wiredtiger.h>

WTSTORE_EXTERN WT_CONNECTION *wtstore_get_database (const char *path, const char *config);
WTSTORE_EXTERN void wtstore_close_database (WT_CONNECTION *connection);

#endif