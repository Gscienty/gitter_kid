#ifndef _G_KID_LOG_
#define _G_KID_LOG_

#include <stdio.h>
#include <time.h>

#define DBG_INFO        "INFO"
#define DBG_WARNING     "WARNING"
#define DBG_ERROR       "ERROR"
#define DBG_CRITICAL    "CRITICAL"

void DBG_LOG (const char *level, const char *fmt);

#endif