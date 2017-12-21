#include <stdio.h>

#ifndef _G_KID_PWIO_
#define _G_KID_PWIO_

struct entry {
    char *line; // store /etc/passwd file's line
    void *ptr;  // store struct passwd
    struct entry *prev;
    struct entry *next;
    int changed : 1;
};

struct ops {
    // make a copy of the object
    // and all strings pointed by it, in malloced memory.
    void* (*dup) (const void *);

    // free the struct passwd
    void (*free) (void *);

    // return the name of the object.
    const char* (*getname) (const void *);

    // parse a string, return object.
    void* (*parse) (const char *);

    // write the object to the file.
    int (*put) (const void *, FILE *);

    char* (*fgets) (char *, int, FILE *);
    int (*fputs) (const char *, FILE *);
};

struct db {
    // name of data file
    char filename[1024];

    struct ops *ops;

    FILE *fp;

    struct entry *head;
    struct entry *tail;
    struct entry *cursor;

    int changed : 1;
    int isopen : 1;
    int locked : 1;
    int readonly : 1;
};

int pw_lock_nowait (struct db *);
int pw_lock (struct db *);

void* __pw_dup (const void *);
void __pw_free (void *);
const char* __pw_getname (const void *);
void* __pw_parse (const char *);
int __pw_put (const void *, FILE *);

int pw_name (struct db *, const char *filename);
int pw_open (struct db *, int mode, int *last_error);
#endif
