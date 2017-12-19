#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fs_access.h"

int access_file_readable (const char* filepath) {
    return access (filepath, R_OK);
}

int access_file_writable (const char* filepath) {
    return access (filepath, W_OK);
}

int access_file_executable (const char* filepath) {
    return access (filepath, X_OK);
}

int access_file_exist (const char* filepath) {
    return access (filepath, F_OK);
}

int access_chmod (const char* path, unsigned int mode) {
    return chmod (path, mode);
}

int access_chown (const char* path, unsigned int owner, unsigned int group) {
    return chown (path, owner, group);
}