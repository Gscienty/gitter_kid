#include "user.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>

#define BUFLEN 1024
int __open_db (struct db *db, int mode) {
    if (db == NULL) {
        // db is not exist
        DBG_LOG (DBG_ERROR, "open_db: db is not exist");
        return -6;
    }
    if (db->isopen || (mode != O_RDONLY && mode != O_RDWR)) {
        // opened
        DBG_LOG (DBG_ERROR, "open_db: file is opened");
        return -1;
    }

    db->readonly = (mode == O_RDONLY);
    if (!db->readonly && !db->locked) {
        // cannot read it
        DBG_LOG (DBG_ERROR, "open_db: db cannot read it");
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
        DBG_LOG (DBG_ERROR, "open_db: file is not exist");
        return -3;
    }
    db->isopen = 1;

    int buflen = BUFLEN;
    char *buf = (char *) malloc (buflen);
    if (!buf) {
        fclose (db->fp);
        // have not enough free memory
        DBG_LOG (DBG_ERROR, "open_db: have not enough free memory");
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
                DBG_LOG (DBG_ERROR, "open_db: have not enough free memory");
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
            DBG_LOG (DBG_ERROR, "open_db: have not free memory");
            return -4;
        }
        void *eptr = db->ops->parse (line);
        if (!eptr) {
            free (line);
            free (buf);
            fclose (db->fp);
            // cannot parse this line
            DBG_LOG (DBG_ERROR, "open_db: cannot parse this line");
            return -5;
        }

        struct entry *p_entry = (struct entry *) malloc (sizeof (*p_entry));

        if (!p_entry) {
            db->ops->free (eptr);
            free (line);
            free (buf);
            fclose (db->fp);
            // have not enough free memory
            DBG_LOG (DBG_ERROR, "open_db: have not enough free memory");
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

void __dispose_db (struct db *db) {
    if (db == NULL) {
        return ;
    }

    struct entry *free_entry = db->head;

    while (free_entry != NULL) {
        free (free_entry->line);
        db->ops->free (free_entry->ptr);
        struct entry *next_entry = free_entry->next;
        free (free_entry);
        free_entry = next_entry;
    }

    fclose (db->fp);

    free (db);
}

struct entry *__find_entry_by_name (struct db *db, const char *name) {
    struct entry *ret = NULL;
    for (ret = db->head; ret; ret = ret->next) {
        void *ep = ret->ptr;
        if (ep && strcmp (db->ops->getname (ret->ptr), name) == 0) {
            break;
        }
    }

    return ret;
}

int __append_db (struct db *db, const void *p) {
    if (!db->isopen || db->readonly) {
        DBG_LOG (DBG_ERROR, "append_db: could not append premission denied");
        return -1;
    }

    void *ptr = db->ops->dup (p);
    if (ptr == NULL) {
        DBG_LOG (DBG_ERROR, "append_db: format error. count not transfer legal format");
        return -2;
    }

    struct entry *entry = __find_entry_by_name (db, db->ops->getname (ptr));
    if (entry != NULL) {
        db->ops->free (ptr);
        DBG_LOG (DBG_WARNING, "append_db: exist same name's entry");
        return -3;
    }

    entry = (struct entry *) malloc (sizeof (*entry));
    if (entry == NULL) {
        db->ops->free (ptr);
        DBG_LOG (DBG_ERROR, "append_db: have not enough free memory");
        return -4;
    }

    entry->ptr = ptr;
    entry->line = NULL;
    entry->changed = 1;
    entry->next = NULL;
    entry->prev = db->tail;
    if (db->head == NULL) {
        db->head = entry;
    }
    if (db->tail != NULL) {
        db->tail->next = entry;
    }
    db->tail = entry;
    db->changed = 1;

    return 0;
}

int __backup (const char *backup, FILE *fp) {
    struct stat sb;
    struct utimbuf ub;
    if (fstat (fileno (fp), &sb)) {
        return -1;
    }
    mode_t mask = umask (077);
    FILE *bkfp = fopen (backup, "w");
    umask (mask);
    if (bkfp == NULL) {
        return -1;
    }
    char c;
    if (fseek (fp, 0, SEEK_SET) == 0) {
        while ((c = getc (fp)) != EOF) {
            if (putc (c, bkfp) == EOF) {
                break;
            }
        }
    }
    if (c != EOF || ferror (fp) || fflush (bkfp)) {
        fclose (bkfp);
        return -1;
    }
    if (fclose (bkfp)) {
        return -1;
    }

    ub.actime = sb.st_atime;
    ub.modtime = sb.st_mtime;
    utime (backup, &ub);
    return 0;
}

FILE *__fopen_perms (const char *name, const char *mode, const struct stat *sb) {
    mode_t mask = umask (0777);
    FILE *fp = fopen (name, mode);
    umask (mask);
    if (fp == NULL) {
        return NULL;
    }

    if (fchown (fileno (fp), sb->st_uid, sb->st_gid)) {
        fclose (fp);
        unlink (name);
        return NULL;
    }
    if (fchmod (fileno (fp), sb->st_mode & 0664)) {
        fclose (fp);
        unlink (name);
        return NULL;
    }

    return fp;
}

int __write_db (const struct db *db) {
    struct entry *p;
    for (p = db->head; p; p = p->next) {
        if (p->changed) {
            void *ptr = p->ptr;
            if (db->ops->put (ptr, db->fp)) {
                return -1;
            }
        }
        else if (p->line) {
            if (db->ops->fputs (p->line, db->fp) == EOF) {
                return -1;
            }
            if (putc ('\n', db->fp) == EOF) {
                return -1;
            }
        }
    }

    return 0;
}

int __save_db (struct db *db) {
    if (db == NULL) {
        return -1;
    }
    if (db->isopen == 0) {
        DBG_LOG (DBG_ERROR, "file is not opend");
        return -2;
    }
    if (db->changed == 0) {
        // success
        return 0;
    }
    if (db->readonly == 1) {
        DBG_LOG (DBG_ERROR, "premission denied. this file cannot write");
        return -3;
    }
    char buf[1024];
    struct stat sb;
    memset (&sb, 0, sizeof (sb));
    if (db->fp) {
        if (fstat (fileno (db->fp), &sb)) {
            fclose (db->fp);
            db->fp = NULL;
            DBG_LOG (DBG_ERROR, "occur error");
            return -4;
        }

        snprintf (buf, sizeof (buf), "%s-", db->filename);
        if (__backup (buf, db->fp) != 0) {
            DBG_LOG (DBG_ERROR, "occur error");
            return -4;
        }
        if (fclose (db->fp)) {
            DBG_LOG (DBG_ERROR, "occur error");
            return -4;
        }
    }
    else {
        sb.st_mode = 0400;
        sb.st_uid = 0;
        sb.st_gid = 0;
    }

    snprintf (buf, sizeof (buf), "%s+", db->filename);
    db->fp = __fopen_perms (buf, "w", &sb);
    if (db->fp == NULL) {
        DBG_LOG (DBG_ERROR, "occur error");
        return -5;
    }
    if (__write_db (db)) {
        DBG_LOG (DBG_ERROR, "occur error");
        return -6;
    }
    if (fflush (db->fp)) {
        DBG_LOG (DBG_ERROR, "occur error");
        return -7;
    }
    if (fsync (fileno (db->fp))) {
        DBG_LOG (DBG_ERROR, "occur error");
        return -8;
    }
    if (fclose (db->fp)) {
        DBG_LOG (DBG_ERROR, "occur error");
        return -9;
    }
    if (rename (buf, db->filename)) {
        DBG_LOG (DBG_ERROR, "occur error");
        return -10;
    }

    return 0;
}