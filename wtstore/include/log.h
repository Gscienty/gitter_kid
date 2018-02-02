#ifndef _WTSTORE_LOG_
#define _WTSTORE_LOG_

#include <stdio.h>
#include <time.h>
#include <wiredtiger.h>

#define DBG_INFO        "INFO"
#define DBG_WARNING     "WARNING"
#define DBG_ERROR       "ERROR"
#define DBG_CRITICAL    "CRITICAL"

void DBG_LOG(const char *level, const char *fmt);

void check_error (int state);

#endif