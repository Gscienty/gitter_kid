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

void check_error (int state) {
    switch (state) {
        case WT_ROLLBACK:
            DBG_LOG (DBG_ERROR, "wt_rollback error: This error is generated when an operation cannot be completed due to a conflict with concurrent operations. The operation may be retried; if a transaction is in progress, it should be rolled back and the operation retried in a new transaction.");
            break;
        case WT_DUPLICATE_KEY:
            DBG_LOG (DBG_ERROR, "wt_duplicate_key error: This error is generated when the application attempts to insert a record with the same key as an existing record without the 'overwrite' configuration to WT_SESSION::open_cursor.");
            break;
        case WT_ERROR:
            DBG_LOG (DBG_ERROR, "wt_error error: This error is returned when an error is not covered by a specific error return.");
            break;
        case WT_NOTFOUND:
            DBG_LOG (DBG_ERROR, "wt_notfound error: This error indicates an operation did not find a value to return. This includes cursor search and other operations where no record matched the cursor's search key such as WT_CURSOR::update or WT_CURSOR::remove.");
            break;
        case WT_PANIC:
            DBG_LOG (DBG_ERROR, "wt_panic error: This error indicates an underlying problem that requires a database restart. The application may exit immediately, no further WiredTiger calls are required (and further calls will themselves immediately fail).");
            break;
        case WT_RUN_RECOVERY:
            DBG_LOG (DBG_ERROR, "wt_run_recovery error: This error is generated when wiredtiger_open is configured to return an error if recovery is required to use the database.");
            break;
        case WT_CACHE_FULL:
            DBG_LOG (DBG_ERROR, "wt_cache_full error: This error is only generated when wiredtiger_open is configured to run in-memory, and an insert or update operation requires more than the configured cache size to complete. The operation may be retried; if a transaction is in progress, it should be rolled back and the operation retried in a new transaction.");
            break;
    }
}