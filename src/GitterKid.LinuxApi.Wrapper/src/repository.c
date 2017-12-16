#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "repository.h"

int create_directory(const char* path) {
    if ( access(path, F_OK) == 0 ) {
        // directory exist
        return -1;
    }

    if ( mkdir(path, GROUP_READABLE | GROUP_WRITABLE) == -1 ) {
        // make directory error
        return -1;
    }
    
    return 0;
}

int repository_init(const char* path) {
    if ( create_directory(path) != 0 ) {
        return -1;
    }

    

    return 0;
}

int repository_remove(const char* path) {

    return 0;
}