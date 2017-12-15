#include "define.h"

G_KID_EXTERN int access_file_readable(const char* filepath);
G_KID_EXTERN int access_file_writable(const char* filepath);
G_KID_EXTERN int access_file_executable(const char* filepath);
G_KID_EXTERN int access_file_exist(const char* filepath);

G_KID_EXTERN int access_chmod(const char* path, unsigned int mode);

G_KID_EXTERN int access_chown(const char* path, unsigned int owner, unsigned int group);