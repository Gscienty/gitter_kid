#include "gitfs.h"
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <limits.h>

DIR *__git_packes_get_dir (const char *repo_path, size_t repo_path_len) {
    char *pack_path = (char *) malloc (sizeof (char) * (repo_path_len + 14));
    if (pack_path == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
        return NULL;
    }
    strcpy (pack_path, repo_path);
    strcpy (pack_path + repo_path_len, "objects/pack/");
    pack_path[repo_path_len + 13] = 0;

    DIR *ret = opendir (pack_path);
    
    free (pack_path);

    return ret;
}

struct git_pack *__git_packes_get_pack (const char *repo_path, size_t repo_path_len, const char *idx_name) {
    struct git_pack *pack = (struct git_pack *) malloc (sizeof (*pack));
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
        return NULL;
    }
    pack->idx_path = (char *) malloc (sizeof (char) * (repo_path_len + 63));
    if (pack->idx_path == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
        free (pack);
        return NULL;
    }
    strcpy (pack->idx_path, repo_path);
    strcpy (pack->idx_path + repo_path_len, "objects/pack/");
    strcpy (pack->idx_path + repo_path_len + 13, idx_name);
    pack->idx_path[repo_path_len + 62] = 0;
    pack->idx_map = NULL;
    pack->idx_fd = 0;
    pack->idx_size = 0;
    pack->count = 0;
    pack->rdtree = NULL;

    return pack;
}

struct git_packes *git_packes_get (struct git_repo *repo) {
    if (repo == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: repo is null");
        return NULL;
    }
    int repo_path_len = strlen (repo->path);
    DIR *dir = __git_packes_get_dir (repo->path, repo_path_len);
    if (dir == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: packes cannot opend");
        return NULL;
    }

    struct git_packes *result = (struct git_packes *) malloc (sizeof (*result));
    if (result == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
        closedir (dir);
        return NULL;
    }
    result->head = result->tail = result->cursor = NULL;
    struct dirent *ent;

    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_REG) {
            if (strcmp (ent->d_name + 45, ".idx") != 0) {
                continue;
            }

            struct git_pack *pack = __git_packes_get_pack (repo->path, repo_path_len, ent->d_name);
            if (pack == NULL) {
                DBG_LOG (DBG_ERROR, "git_packes_get: cannot get current pack");
                git_packes_dispose (result);
                closedir (dir);
                return NULL;
            }

            pack->next = NULL;
            pack->prev = result->tail;
            if (result->head == NULL) {
                result->head = pack;
            }
            if (result->tail != NULL) {
                result->tail->next = pack;
            }
            result->tail = pack;
        }
    }
    closedir (dir);
    return result;
}

void git_packes_dispose (struct git_packes *packes) {
    if (packes == NULL) {
        return ;
    }
    while (packes->head) {
        struct git_pack *next_pack = packes->head->next;
        free (packes->head->idx_path);
        if (packes->head->idx_map != NULL) {
            munmap (packes->head->idx_map, packes->head->idx_size);
        }
        if (packes->head->idx_fd != 0) {
            close (packes->head->idx_fd);
        }
        free (packes->head);
        packes->head = next_pack;
    }
    free (packes);

    // TODO: need dispost other fields
}

void git_packes_reset (struct git_packes *packes) {
    if (packes == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_reset: packes is null");
        return ;
    }
    packes->cursor = packes->head;
}

struct git_pack *git_packes_get_current (struct git_packes *packes) {
    if (packes == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get_current: packes is null");
        return NULL;
    }
    return packes->cursor;
}

int git_packes_move_next (struct git_packes *packes) {
    if (packes == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_move_next: packes is null");
        return -1;
    }
    if (packes->cursor == NULL) {
        DBG_LOG (DBG_INFO, "git_packes_move_next: packes's cursor is null");
        return -1;
    }

    return (packes->cursor = packes->cursor->next) == NULL ? -1 : 0;
}


int git_pack_is_open (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "git_pack_is_open: pack is null");
        return 0;
    }
    return pack->idx_map != NULL;
}

int __git_pack_count (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_count: pack is null");
        return 0;
    }
    if (pack->idx_map == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_count: have not open idx file");
        return 0;
    }

    unsigned int *p = pack->idx_map + 8;
    int i;
    int count = 0;
    for (i = 0; i < 256; i++) {
        int n = ntohl(p[i]);
        if (n > count) {
            count = n;
        }
    }

    return count;
}

int git_pack_open (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "git_pack_open: pack is null");
        return -1;
    }

    pack->idx_fd = open (pack->idx_path, O_RDONLY);
    if (pack->idx_fd == -1) {
        DBG_LOG (DBG_ERROR, "git_pack_open: idx file cannnot opened");
        return -2;
    }
    struct stat st;
    int retval = fstat (pack->idx_fd, &st);
    if (retval != 0) {
        DBG_LOG (DBG_ERROR, "git_pack_open: idx file cannot got stat");
        return -3;
    }
    pack->idx_size = st.st_size;
    pack->idx_map = mmap (NULL, st.st_size, PROT_READ, MAP_SHARED, pack->idx_fd, 0);
    if (pack->idx_map == (void *) -1) {
        close (pack->idx_fd);
        pack->idx_fd = 0;
        pack->idx_map = NULL;
        DBG_LOG (DBG_ERROR, "git_pack_open: idx file cannot mmap");
        return -4;
    }
    pack->count = __git_pack_count (pack);
    if (pack->count == 0) {
        close (pack->idx_fd);
        pack->idx_fd = 0;
        pack->idx_map = NULL;
        DBG_LOG (DBG_ERROR, "git_pack_open: idx file cannot got count");
        return -5;
    }

    __git_pack_build_rdtree (pack);
    return 0;
}

void *__git_pack_get_nth_signture (struct git_pack *pack, int n) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_nth_signture: pack is null");
        return NULL;
    }
    if (pack->idx_map == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_nth_signture: pack need open idx file");
        return NULL;
    }
    return pack->idx_map + 8 + 1024 + 20 * n;
}

int __git_pack_get_nth_offset (struct git_pack *pack, int n) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_nth_signture: pack is null");
        return 0;
    }
    if (pack->idx_map == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_get_nth_signture: pack need open idx file");
        return 0;
    }

    unsigned int *retval = (int *) (pack->idx_map + 8 + 1024 + 24 * pack->count + 4 * n);
    return ntohl (*retval);
}

void __git_pack_qsort_indexes (struct git_pack *pack, int *indexes, int start, int end) {
    if (start >= end) {
        return;
    }

    int i = start;
    int j = end;
    int k = indexes[start];
    while (i < j) {
        while (i < j && __git_pack_get_nth_offset (pack, k) < __git_pack_get_nth_offset (pack, indexes[j])) {
            j--;
        }
        indexes[i] = indexes[j];
        while (i < j && __git_pack_get_nth_offset (pack, indexes[i]) < __git_pack_get_nth_offset (pack, k)) {
            i++;
        }
        indexes[j] = indexes[i];
    }
    indexes[i] = k;

    __git_pack_qsort_indexes (pack, indexes, start, i - 1);
    __git_pack_qsort_indexes (pack, indexes, i + 1, end);
}

int *__git_pack_get_sorted_indexes (struct git_pack *pack) {
    int *indexes = (int *) malloc (sizeof (*indexes) * pack->count);
    int i;
    for (i = 0; i < pack->count; i++) {
        indexes[i] = i;
    }
    __git_pack_qsort_indexes (pack, indexes, 0, pack->count - 1);

    return indexes;
}

void __git_pack_build_rdtree (struct git_pack *pack) {
    if (pack == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_build_rdtree: pack is null");
        return;
    }
    if (pack->idx_map == NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_build_rdtree: pack need open idx file");
        return;
    }
    if (pack->rdtree != NULL) {
        DBG_LOG (DBG_ERROR, "__git_pack_build_rdtree: already exist red black tree");
        return;
    }


    int base_len = strlen (pack->idx_path);
    char *packfile_path = (char *) malloc (sizeof (char) * (base_len + 2));
    strncpy (packfile_path, pack->idx_path, base_len - 3);
    strcpy (packfile_path + base_len - 3, "pack");

    struct stat st;
    if (stat (packfile_path, &st) != 0) {
        DBG_LOG (DBG_ERROR, "__git_pack_build_rdtree: cannot get .pack file stat");
        free (packfile_path);
        return;
    }
    free (packfile_path);

    int *indexes = __git_pack_get_sorted_indexes (pack);
    pack->rdtree = rdt_build ();
    int i;
    for (i = 0; i < pack->count; i++) {
        rdt_insert (
            pack->rdtree,
            __git_pack_get_nth_signture (pack, indexes[i]),
            __git_pack_get_nth_offset (pack, indexes[i]),
            i + 1 == pack->count
                ? st.st_size - __git_pack_get_nth_offset (pack, indexes[i])
                : __git_pack_get_nth_offset (pack, indexes[i + 1]) - __git_pack_get_nth_offset (pack, indexes[i])
        );
    }

    free (indexes);
}
