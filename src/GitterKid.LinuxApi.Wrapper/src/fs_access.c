#include <unistd.h>
#include "fs_access.h"

int access_file_readable(const char* filepath) {
    return access(filepath, R_OK);
}

int access_file_writable(const char* filepath) {
    return access(filepath, W_OK);
}

int access_file_executable(const char* filepath) {
    return access(filepath, X_OK);
}

int access_file_exist(const char* filepath) {
    return access(filepath, F_OK);
}