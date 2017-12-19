
struct ops {
    // make a copy of the object
    // and all strings pointed by it, in malloced memory.
    void* (*dup) (const void *);

    // free the object including any strings pointed by it.
    void (*free) (void *);

    // return the name of the object.
    const char* (*getname) (const void *);

    // parse a string, return object.
    void* (*parse) (const char *);

    // write the object to the file.
    int (*put) (const void*, FILE*);

    char* (*fgets) (char*, int, FILE*);
    int (*fputs) (const char*, FILE*);
};

struct db {
    // name of data file
    char filename[1024];

    struct ops* ops;

    FILE* fp;

    int changed : 1;
    int isopen : 1;
    int locked : 1;
    int readonly : 1;
};

int lock_nowait (struct db*);
int lock (struct db*);