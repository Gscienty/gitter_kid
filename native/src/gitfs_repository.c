#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <malloc.h>
#include "gitfs.h"

char* __path_join (const char* basepath, size_t basepath_length, const char* subpath) {
    size_t response_path_length = sizeof (char) * (basepath_length + strlen (subpath) + 1);

    char* response_path = (char*) malloc (response_path_length);
    memset (response_path, 0, sizeof (response_path));

    strcat (response_path, basepath);
    strcat (response_path, subpath);

    return response_path;
}

/*
* create directory
* if success then return 0
* if directory exist then return -1
* if make directory error then return -2
*/
int __create_directory (const char* path) {
    if ( access (path, F_OK) == 0 ) {
        // directory exist
        return -1;
    }

    if ( mkdir (path, USER_READABLE | USER_WRITABLE | USER_EXECUTABLE | GROUP_WRITABLE | GROUP_READABLE) == -1 ) {
        // make directory error
        return -2;
    }
    
    return 0;
}

/*
* if could not open this file then return -3
* if success then return 0
*/
int __create_file (
    const char* directory_path,
    const size_t path_length,
    const char* filename,
    const char* content) {
        char* path = __path_join (directory_path, path_length, filename);

        FILE* file = fopen (path, "w+");
        if ( file == NULL ) {
            // could not open this file
            return -3;
        }

        fputs (content, file);
        chmod (path, USER_READABLE | USER_WRITABLE | USER_EXECUTABLE | GROUP_WRITABLE | GROUP_READABLE);

        free (path);
        fclose (file);
        return 0;
}

/*
* create sub directory
* if success then return 0
* if directory exist then return -1
* if make directory error then return -2
*/
int __create_sub_directory (const char* path, const size_t path_length, const char* subpath) {
    char* branches_path = __path_join (path, path_length, subpath);

    int retval = __create_directory (branches_path);
    if (retval != 0) {
        free (branches_path);
        return retval;
    }

    free (branches_path);
    return 0;
}

#define CREATE_SUB_DIRECTORY(retval, subpath) \
    retval = __create_sub_directory(path, path_length, subpath); \
    if (retval != 0) { \
        return retval; \
    }

#define CREATE_ROOT_FILE(retval, filename, content) \
    retval = __create_file(path, path_length, filename, content); \
    if (retval != 0) { \
        return retval; \
    }


/*
* create sub directory
* if success then return 0
* if directory exist then return -1
* if make directory error then return -2
* if could not open this file then return -3
*/
int __repository_init (const char* path, const char* description) {
    int retval = 0;
    // create repository directory
    retval = __create_directory (path);
    if (retval != 0) {
        return retval;
    }

    size_t path_length = strlen (path);

    // create repository's branches sub directory
    CREATE_SUB_DIRECTORY (retval, "/branches")

    // create repository's hooks sub directory
    CREATE_SUB_DIRECTORY (retval, "/hooks")

    // create repository's info sub directory
    CREATE_SUB_DIRECTORY (retval, "/info")

    // create repository's objects sub directory
    CREATE_SUB_DIRECTORY (retval, "/objects")
    CREATE_SUB_DIRECTORY (retval, "/objects/info")
    CREATE_SUB_DIRECTORY (retval, "/objects/pack")

    // create repository's refs sub directory
    CREATE_SUB_DIRECTORY (retval, "/refs")
    CREATE_SUB_DIRECTORY (retval, "/refs/heads")
    CREATE_SUB_DIRECTORY (retval, "/refs/tags")

    // create config file
    CREATE_ROOT_FILE (retval, "/config", "[core]\trepositoryformatversion = 0\n\tfilemode = false\n\tbare = true\n")

    // create description file
    CREATE_ROOT_FILE (retval, "/description", description)

    // create HEAD file
    CREATE_ROOT_FILE (retval, "/HEAD", "ref: refs/heads/master\n")

    return 0;
}

int __repository_remove (const char* path) {

    if (access (path, F_OK) != 0) {
        // directory not exist
        return -1;
    }

    return rmdir (path);
}