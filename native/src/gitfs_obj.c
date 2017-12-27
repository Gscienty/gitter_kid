#include "gitfs.h"
#include <string.h>
#include <malloc.h>
#include <zlib.h>

void *zalloc (void *q, unsigned int n, unsigned int m) {
    q = NULL;

    return calloc (n, m);
}

void *zfree (void *q, void *p) {
    q = NULL;

    free (p);
}

struct git_obj *git_obj_get (struct git_repo *repo, const char* signture, enum git_obj_type type) {
    if (repo == NULL) {
        return NULL;
    }
    if (signture == NULL) {
        return NULL;
    }
    if (strlen (signture) != 40) {
        // sign illegal
        return NULL;
    }

    register char *sign_ch;

    for (sign_ch = signture; *sign_ch; sign_ch++) {
        if (!(('0' <= *sign_ch && *sign_ch <= '9') || ('a' <= *sign_ch && *sign_ch <= 'f'))) {
            // sign illegal
            return NULL;
        }
    }

    int path_length = strlen (repo->path);

    char *obj_file_path = (char *) malloc (sizeof (char) * (path_length + 50));
    strcpy (obj_file_path, repo->path);
    strcpy (obj_file_path + path_length, "objects/");
    strncpy (obj_file_path + path_length + 8, signture, 2);
    strcpy (obj_file_path + path_length + 10, "/");
    strcpy (obj_file_path + path_length + 11, signture + 2);

    FILE *zipfile = fopen (obj_file_path, "rb");
    if (zipfile == NULL) {
        return NULL;
    }

    fseek (zipfile, 0, SEEK_END);
    int flen = ftell (zipfile);
    fseek (zipfile, 0, SEEK_SET);

    unsigned char *zfbuf = malloc (sizeof (*zfbuf) * flen);
    if (zfbuf == NULL) {
        return NULL;
    }

    fread (zfbuf, sizeof (unsigned char), flen, zipfile);
    
    z_stream d_stream;
    d_stream.zalloc = zalloc;
    d_stream.zfree = zfree;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in = zfbuf;
    d_stream.avail_in = (unsigned int) flen;

    int err = inflateInit (&d_stream);
    int unzip_buf_length = sizeof (unsigned char *) * (flen * 3 + 1024);
    unsigned char *unzipbuf = (unsigned char *) malloc (unzip_buf_length);
    d_stream.next_out = unzipbuf;
    d_stream.avail_out = (unsigned int) unzip_buf_length;

    inflate (&d_stream, Z_NO_FLUSH);

    inflateEnd (&d_stream);

    printf ("%s, %s\n", unzipbuf, unzipbuf + strlen (unzipbuf) + 1);
}