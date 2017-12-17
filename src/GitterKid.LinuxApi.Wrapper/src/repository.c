#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "repository.h"

char* __path_join(const char* basepath, size_t basepath_length, const char* subpath) {
    size_t subpath_length = strlen(subpath);

    char* response_path = (char*)malloc(sizeof(char) * (basepath_length + subpath_length + 1));

    memccpy(response_path, basepath, 0, basepath_length);
    memccpy(response_path + basepath_length, subpath, 0, subpath_length);

    return response_path;
}

int __create_directory(const char* path) {
    if ( access(path, F_OK) == 0 ) {
        // directory exist
        return -1;
    }

    if ( mkdir(path, USER_READABLE | USER_WRITABLE | USER_EXECUTABLE | GROUP_WRITABLE | GROUP_READABLE) == -1 ) {
        // make directory error
        return -1;
    }
    
    return 0;
}

int __create_file(
    const char* directory_path,
    const size_t path_length,
    const char* filename,
    const char* content) {
        char* path = __path_join(directory_path, path_length, filename);

        FILE* file = fopen(path, "w+");
        if ( file == NULL ) {
            // could not open this file
            return -1;
        }

        fputs(content, file);

        free(path);
        fclose(file);
        return 0;
}

int __create_sub_directory(const char* path, const size_t path_length, const char* ) {
    char* branches_path = ___path_join(path, path_length, subpath);
    if ( __create_directory(branches_path) != 0 ) {
        free(branches_path);
        return -1;
    }
    free(branches_path);
}

#define CREATE_SUB_DIRECTORY(subpath) \
    if ( __create_sub_directory(path, path_length, subpath) != 0 ) { \
        return -1; \
    }

#define CREATE_ROOT_FILE(filename, content) \
    if ( __create_file(path, path_length, filename, content) != 0 ) { \
        return -1; \
    }

int repository_init(const char* path, const char* description) {
    // create repository directory
    if ( __create_directory(path) != 0 ) {
        return -1;
    }

    size_t path_length = strlen(path);

    // create repository's branches sub directory
    CREATE_SUB_DIRECTORY("/branches")

    // create repository's hooks sub directory
    CREATE_SUB_DIRECTORY("/hooks")

    // create repository's info sub directory
    CREATE_SUB_DIRECTORY("/info")

    // create repository's objects sub directory
    CREATE_SUB_DIRECTORY("/objects")
    CREATE_SUB_DIRECTORY("/objects/info")
    CREATE_SUB_DIRECTORY("/objects/pack")

    // create repository's refs sub directory
    CREATE_SUB_DIRECTORY("/refs")
    CREATE_SUB_DIRECTORY("/refs/heads")
    CREATE_SUB_DIRECTORY("/refs/tags")

    // create config file
    CREATE_ROOT_FILE("/config", "[core]\trepositoryformatversion = 0\n\tfilemode = false\n\tbare = true\n")

    // create description file
    CREATE_ROOT_FILE("/description", description)

    // create HEAD file
    CREATE_ROOT_FILE("/HEAD", "ref: refs/heads/master\n")

    return 0;
}

int repository_remove(const char* path) {

    return 0;
}