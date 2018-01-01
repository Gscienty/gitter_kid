#include "gitfs.h"
#include <malloc.h>
#include <string.h>
#include <dirent.h>

struct git_market *git_market_build (const char *basepath) {
    if (__access_file_exist (basepath) != 0) {
        // path not exist
        return NULL;
    }
    struct git_market *ret = (struct git_market *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // not enough free memory
        return NULL;
    }
    if (!(ret->path = strdup (basepath))) {
        // not enough free memory
        free (ret);
        return NULL;
    }

    ret->head = ret->tail = ret->cursor = NULL;

    struct dirent *ent = NULL;
    DIR *dir = opendir (basepath);
    if (dir == NULL) {
        free (ret);
        return NULL;
    }
    int basepath_length = strlen (basepath);
    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_DIR && ent->d_name[0] != '.') {
            struct git_repo *repo = (struct git_repo *) malloc (sizeof (*repo));
            if (repo == NULL) {
                goto free_repo_list;
            }
            repo->name = strdup (ent->d_name);
            if (repo->name == NULL) {
                free (repo->name);
                free (repo);
                goto free_repo_list;
            }
            int repo_name_length = strlen (ent->d_name);
            repo->path = (char *) malloc (sizeof (char) * (basepath_length + repo_name_length + 3));
            if (repo->path == NULL) {
                free (repo->name);
                free (repo->path);
                free (repo);
                goto free_repo_list;
            }
            strcpy (repo->path, basepath);
            if (repo->path[basepath_length - 1] != '/') {
                strcpy (repo->path + basepath_length, "/");
                strcpy (repo->path + basepath_length + 1, ent->d_name);
                strcpy (repo->path + basepath_length + repo_name_length + 1, "/");
            }
            else {
                strcpy (repo->path + basepath_length, ent->d_name);
                strcpy (repo->path + basepath_length + repo_name_length, "/");
            }
            repo->prev = ret->tail;
            repo->next = NULL;

            if (ret->head == NULL) {
                ret->head = repo;
            }
            if (ret->tail != NULL) {
                ret->tail->next = repo;
            }
            ret->tail = repo;
        }
    }

    return ret;
    // free repo list
    free_repo_list:
    git_market_dispose (ret);
    return NULL;
}

void git_market_dispose (struct git_market *market) {
    struct git_repo *repo = market->head;

    while (repo) {
        free (repo->path);
        free (repo->name);

        struct git_repo *next_repo = repo->next;
        free (repo);
        repo = next_repo;
    }

    free (market->path);
    free (market);
}

int git_market_cursor_move_next (struct git_market *market) {
    if (market == NULL) {
        DBG_LOG (DBG_ERROR, "git_market_cursor_move_next: market is null");
        return -1;
    }
    if (market->cursor == NULL) {
        DBG_LOG (DBG_ERROR, ":git_market_cursor_move_net: market's cursor is null");
        return -1;
    }

    return (market->cursor = market->cursor->next) == NULL ? -1 : 0;
}

struct git_repo *git_market_cursor_current (const struct git_market *market) {
    if (market == NULL) {
        return NULL;
    }
    return market->cursor;
}

void git_market_cursor_reset (struct git_market *market) {
    if (market == NULL) {
        return ;
    }
    market->cursor = market->head;
}

G_KID_EXTERN char *git_repo_path (struct git_repo *repo) {
    if (repo == NULL) {
        return NULL;
    }
    return repo->path;
}

G_KID_EXTERN char *git_repo_name (struct git_repo *repo) {
    if (repo == NULL) {
        return NULL;
    }
    return repo->name;
}