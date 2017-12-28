#include "log.h"

void DBG_LOG(const char *level, const char *fmt) {
#ifdef DBG
    do {
    time_t t = time (NULL);
    struct tm *tmm = localtime (&t);
    printf ("[%d-%d-%d %d:%d:%d] [ %s ] %s \n", tmm->tm_year + 1900, tmm->tm_mon + 1, tmm->tm_mday, tmm->tm_hour, tmm->tm_min, tmm->tm_sec, level, fmt);
    } while (0);
#endif
}