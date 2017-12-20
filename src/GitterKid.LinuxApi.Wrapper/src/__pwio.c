#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <string.h>
#include <shadow.h>
#include "__pwio.h"

static int lock_count = 0;

int __check_link_count (const char* file) {
    struct stat st;

    if ( stat (file, &st) != 0 ) {
        return 0;
    }

    if ( st.st_nlink != 2 ) {
        return 0;
    }

    return 1;
}

int __lock_file (const char* file, const char* lock) {
    int fd;
    int len;

    fd = open (file, O_CREAT | O_EXCL | O_WRONLY, 0600);
    if ( fd == -1 ) {
        return 0;
    }
    int pid = getpid ();
    char buffer[32];
    snprintf (buffer, sizeof (buffer), "%d", pid);
    len = strlen (buffer) + 1;
    if ( write (fd, buffer, len) != len ) {
        close (fd);
        unlink (file);
        return 0;
    }
    close (fd);

    int retval;

    if ( link (file, lock) == 0 ) {
        retval = __check_link_count (file);
        unlink (file);
        return retval;
    }

    fd = open (lock, O_RDWR);
    if ( fd == -1 ) {
        unlink (file);
        return retval;
    }
    len = read (fd, buffer, sizeof (buffer) - 1);
    close (fd);

    if ( len <= 0 ) {
        unlink (file);
        return 0;
    }
    buffer[len] = 0;
    pid = strtol(buffer, (char **) 0, 10);
    if ( pid == 0 ) {
        unlink (file);
        return 0;
    }
    if ( kill (pid, 0) == 0 ) {
        unlink (file);
        return 0;
    }
    if ( unlink (lock) != 0 ) {
        unlink (file);
        return 0;
    }

    retval = 0;
    if ( link (file, lock) == 0 && __check_link_count (file) ) {
        retval = 1;
    }

    unlink (file);

    return retval;
}

int pw_lock_nowait (struct db* db) {
    char file[1024];
    char lock[1024];

    if ( db->locked ) {
        return 1;
    }

    snprintf (file, sizeof (file), "%s.%ld", db->filename, (long) getpid () );
    snprintf (lock, sizeof (lock), "%s.lock", db->filename);

    if ( __lock_file (file, lock) ) {
        db->locked = 1;
        lock_count++;
        return 1;
    }
    return 0;
}

int pw_lock (struct db* db) {
    if ( lock_count == 0 ) {
        if ( lckpwdf () == -1 ) {
            return 0;
        }
    }

    if ( pw_lock_nowait (db) ) {
        return 1;
    }

    ulckpwdf ();
    return 0;
}

void* __pw_dup (const void* pwent) {
    const struct passwd* pw = (const struct passwd*) pwent;

    struct passwd* retpw = (struct passwd*) malloc (sizeof (*retpw));

    if ( retpw == NULL) {
        return NULL;
    }

    *retpw = *pw;
    retpw->pw_name = strdup(pw->pw_name);
    if ( retpw->pw_name == NULL ) {
        return NULL;
    }
    retpw->pw_passwd = strdup(pw->pw_passwd);
    if ( retpw->pw_passwd == NULL ) {
        return NULL;
    }
    retpw->pw_gecos = strdup(pw->pw_gecos);
    if ( retpw->pw_gecos == NULL ) {
        return NULL;
    }
    retpw->pw_dir = strdup(pw->pw_dir);
    if ( retpw->pw_dir == NULL ) {
        return NULL;
    }
    retpw->pw_shell = strdup(pw->pw_shell);
    if ( retpw->pw_shell == NULL ) {
        return NULL;
    }

    return (void*) retpw;
}

void __pw_free (void* ent) {
    struct passwd* pw = (struct passwd*) ent;

    free (pw->pw_name);
    free (pw->pw_passwd);
    free (pw->pw_gecos);
    free (pw->pw_dir);
    free (pw->pw_shell);
    free (pw);
}

const char* __pw_getname (const void* ent) {
    const struct passwd* pw = (struct passwd*) ent;

    return pw->pw_name;
}

void* __pw_parse (const char* line) {
    struct passwd* pwent = (struct passwd*) malloc (sizeof (*pwent));
    static char pwdbuf[1024];
    register int i;
    register char* cp;
    char* ep;
    char* fields[7];

    if ( strlen (line) >= sizeof (pwdbuf) ) {
        return 0;
    }

    strcpy (pwdbuf, line);

    for (cp = pwdbuf, i = 0; i < 7 && cp; i++) {
        fields[i] = cp;
        while (*cp && *cp != ':') {
            cp++;
        }

        if (*cp) {
            *cp = 0;
            cp++;
        }
        else {
            cp = 0;
        }
    }

    if ( i != 7 || *fields[2] == 0 || *fields[3] == 0 ) {
        return NULL;
    }

    pwent->pw_name = fields[0];
    pwent->pw_passwd = fields[1];
    if ( fields[2][0] == 0
        || ((pwent->pw_uid = strtol (fields[2], &ep, 10)) == 0) && *ep ) {
        return NULL;
    }
    if ( fields[3][0] == 0
        || ((pwent->pw_gid = strtol (fields[3], &ep, 10)) == 0) && *ep ) {
        return NULL;
    }
    pwent->pw_gecos = fields[4];
    pwent->pw_dir = fields[5];
    pwent->pw_shell = fields[6];

    return pwent;
}

int __pw_put (const void* ent, FILE* file) {
    const struct passwd *pw = (const struct passwd*) ent;
    return (putpwent(pw, file)  == -1 ? -1 : 0);
}

int pw_name (struct db *db, const char *filename) {
    snprintf (db->filename, sizeof (db->filename), "%s", filename);
    return 1;
}

#define BUFLEN 4096

/*
* open file /etc/passwd
*/
int pw_open (struct db* db, int mode, int* last_error) {
    // initialize passwd db
    mode &= ~O_CREAT;
    if ( db->isopen || (mode != O_RDONLY && mode != O_RDWR) ) {
        // db is open
        *last_error = -1;
        return 0;
    }
    db->readonly = (mode == O_RDONLY);
    if ( !db->readonly && !db->locked ) {
        // could not read
        *last_error = -2;
        return 0;
    }
    db->head = db->tail = db->cursor = NULL;
    db->changed = 0;
    db->fp = fopen (db->filename, db->readonly ? "r" : "r+");
    if ( !db->fp ) {
        if (mode & O_CREAT) {
            db->isopen = 1;
            return 1;
        }
        // not exist
        *last_error = -3;
        return 0;
    }

    // read file fill to db object
    int buflen = BUFLEN;
    char *buf = (char *) malloc (buflen);
    if ( !buf ) {
        fclose (db->fp);
        db->fp = NULL;
        // could not alloc enough memory
        *last_error = -4;
        return 0;
    }

    // get /etc/passwd line
    while ( db->ops->fgets (buf, buflen, db->fp) ) {
        char *cp;
        // if not contain \n and file content not end (have not read entired line) then extend buffer
        while ( !(cp = strrchr(buf, '\n')) && !feof (db->fp) ) {
            buflen += BUFLEN;
            cp = (char *) realloc (buf, buflen);
            if ( !cp ) {
                free (buf);
                fclose (db->fp);
                db->fp = NULL;

                // could not alloc enough memory
                *last_error = -4;
                return 0;
            }
            buf = cp;
            int len = strlen (buf);
            db->ops->fgets (buf + len, buflen - len, db->fp);
        }
        if ( (cp = strrchr (buf, '\n')) ) {
            *cp = 0;
        }

        // copy buf to line (string)
        char *line = strdup (buf);
        if ( !line ) {
            free (buf);
            fclose (db->fp);
            db->fp = NULL;
            // could not alloc enough memory
            *last_error = -4;
            return 0;
        }
        void *eptr;
        if ( line[0] == '+' || line[0] == '-' ) {
            eptr = NULL;
        }
        else if ( (eptr = db->ops->parse (line)) ) {
            // transfer string line to passwd entry
            eptr = db->ops->dup (eptr);
            if ( !eptr ) {
                free (line);
                free (buf);
                fclose (db->fp);
                db->fp = NULL;
                // could not alloc enough memory
                *last_error = -4;
                return 0;
            }
        }

        // create db entry
        struct entry *p = (struct entry *) malloc (sizeof (*p));
        if ( !p ) {
            if ( eptr ) {
                db->ops->free (eptr);
            }
            free (line);
            free (buf);
            fclose (db->fp);
            db->fp = NULL;
            // could not alloc enough memory
            *last_error = -4;
            return 0;
        }

        p->ptr = eptr;
        p->line = line;
        p->changed = 0;

        p->next = NULL;
        p->prev = db->tail;
        if ( !db->head ) {
            db->head = p;
        }
        if ( db->tail ) {
            db->tail->next = p;
        }
        db->tail = p;
    }

    free (buf);
    db->isopen = 1;
    return 1;
}