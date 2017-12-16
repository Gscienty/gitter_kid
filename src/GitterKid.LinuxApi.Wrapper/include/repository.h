#include "define.h"

#define ALLUSER_READABLE    0400
#define ALLUSER_WRITABLE    0200
#define ALLUSER_EXECUTABLE  0100

#define GROUP_READABLE      0040
#define GROUP_WRITABLE      0020
#define GROUP_EXECUTABLE    0010

#define OTHER_READABLE      0004
#define OTHER_WRITABLE      0002
#define OTHER_EXECUTABLE    0001

G_KID_EXTERN int repository_init(const char* path);

G_KID_EXTERN int repository_remove(const char* path);