#include "__io.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 1024
int open_db (struct db *db, int mode) {
    if (db == NULL) {
        // db is not exist
        return -6;
    }
    if (db->isopen || (mode != O_RDONLY && mode != O_RDWR)) {
        // opened
        return -1;
    }

    db->readonly = (mode == O_RDONLY);
    if (!db->readonly && !db->locked) {
        // cannot read it
        return -2;
    }

    db->head = db->tail = db->cursor = NULL;
    db->changed = 0;
    db->fp = fopen (db->filename, db->readonly ? "r" : "r+");

    if ( !db->fp ) {
        if ( (mode & O_CREAT) ) {
            db->isopen = 1;
            return 0;
        }
        // file not exist
        return -3;
    }

    int buflen = BUFLEN;
    char *buf = (char *) malloc (buflen);
    if (!buf) {
        fclose (db->fp);
        // have not enough free memory
        return -4;
    }

    while (db->ops->fgets (buf, buflen, db->fp)) {
        char *cp;
        while (!(cp = strrchr (buf, '\n')) && !feof (db->fp)) {
            buflen += BUFLEN;
            cp = (char *) realloc (buf, buflen);
            if (!cp) {
                free (buf);
                fclose (db->fp);
                // have not enough free memory
                return -4;
            }
            buf = cp;
            int len = strlen (buf);
            db->ops->fgets (buf + len, buflen - len, db->fp);
        }
        if ((cp = strrchr (buf, '\n'))) {
            *cp = 0;
        }

        char *line = strdup (buf);
        if (!line) {
            free (buf);
            fclose (db->fp);
            // have not enough free memory
            return -4;
        }
        void *eptr = db->ops->parse (line);
        if (!eptr) {
            free (line);
            free (buf);
            fclose (db->fp);
            // cannot parse this line
            return -5;
        }

        struct entry *p_entry = (struct entry *) malloc (sizeof (*p_entry));

        if (!p_entry) {
            db->ops->free (eptr);
            free (line);
            free (buf);
            fclose (db->fp);
            // have not enough free memory
            return -4;
        }

        p_entry->ptr = eptr;
        p_entry->line = line;
        p_entry->changed = 0;
        p_entry->prev = db->tail;
        p_entry->next = NULL;
        if (!db->head) {
            db->head = p_entry;
        }
        if (db->tail) {
            db->tail->next = p_entry;
        }
        db->tail = p_entry;
    }

    free (buf);
    db->isopen = 1;
    return 0;
}