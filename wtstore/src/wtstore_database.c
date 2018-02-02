#include "wtstore_database.h"
#include <malloc.h>
#include <string.h>

WT_CONNECTION *wtstore_get_database(const char *path, const char *config) {
    if (path == NULL) {
        DBG_LOG (DBG_ERROR, "wtstore_get_database: path is null");
        return NULL;
    }

    char *complate_config = NULL;
    if (config != NULL && strlen (config) != 0) {
        const size_t create_len = strlen ("create");
        complate_config = (char *) malloc (create_len + strlen (config) + 1);
        if (complate_config == NULL) {
            DBG_LOG (DBG_ERROR, "wtstore_get_database: have not enough free memory");
            return NULL;
        }
        strcpy (complate_config, "create");
        strcpy (complate_config + create_len, config);
    }
    else {
        complate_config = strdup (config);
    }

    WT_CONNECTION *connection;
    check_error (wiredtiger_open (path, NULL, complate_config, &connection));

    free (complate_config);
    return connection;
}

void wtstore_close_database (WT_CONNECTION *connection) {
    if (connection == NULL) {
        DBG_LOG (DBG_WARNING, "wtstore_close_database: connection is null");
        return NULL;
    }

    connection->close (connection, NULL);
}