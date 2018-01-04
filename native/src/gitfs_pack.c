#include "gitfs.h"
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/mman.h>

struct git_packes *git_packes_get (struct git_repo *repo) {
    if (repo == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: repo is null");
        return NULL;
    }

    int repo_path_length = strlen (repo->path);
    char *pack_path = (char *) malloc (sizeof (*pack_path) * (repo_path_length + 14));
    if (pack_path == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
        return NULL;
    }
    strcpy (pack_path, repo->path);
    strcpy (pack_path + repo_path_length, "objects/pack/");
    pack_path[repo_path_length + 13] = 0;

    struct git_packes *result = (struct git_packes *) malloc (sizeof (*result));
    if (result == NULL) {
        DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
        free (pack_path);
        return NULL;
    }
    result->head = result->tail = result->cursor = NULL;
    struct dirent *ent;
    DIR *dir = opendir (pack_path);
    free (pack_path);
    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_REG) {
            struct git_pack *pack = (struct git_pack *) malloc (sizeof (*pack));
            if (pack == NULL) {
                DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
                git_packes_dispose (result);
                closedir (dir);
                return NULL;
            }
            pack->idx_name = strdup (ent->d_name);
            if (pack->idx_name == NULL) {
                DBG_LOG (DBG_ERROR, "git_packes_get: have not enough free memory");
                git_packes_dispose (result);
                closedir (dir);
                free (pack);
                return NULL;
            }
            pack->idx_map = NULL;
            pack->offset = 0;
            pack->length = 0;
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
    close (dir);

    return result;
}

void git_packes_dispose (struct git_packes *packes) {
    if (packes == NULL) {
        return ;
    }
    while (packes->head) {
        struct git_pack *next_pack = packes->head->next;
        free (packes->head->idx_name);
        munmap (packes->head->idx_map, packes->head->length);
        free (packes->head);
        packes->head = next_pack;
    }
    free (packes);
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
