#include "gitfs.h"
#include <malloc.h>
#include <string.h>
#include <dirent.h>

char *__gitrepo_get_path (const char *basepath, size_t basepath_len, const char *reponame, size_t reponame_len) {
    char *ret = (char *) malloc (basepath_len + reponame_len + 3);
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitrepo_get_path: have not enough free memory");
        return NULL;
    }

    strcpy (ret, basepath);
    if (basepath[basepath_len - 1] != '/') {
        strcpy (ret + basepath_len, "/");
        basepath_len++;
    }
    strcpy (ret + basepath_len, reponame);
    strcpy (ret + basepath_len + reponame_len, "/");

    return ret;
}

void __gitmarket_append (struct gitmarket *market, struct gitrepo* repo) {
    repo->packes = NULL;
    repo->prev = market->tail;
    repo->next = NULL;

    if (market->head == NULL) {
        market->head = repo;
    }
    if (market->tail != NULL) {
        market->tail->next = repo;
    }
    market->tail = repo;
}

struct gitmarket *get_gitmarket (const char *basepath) {
    if (__access_file_exist (basepath) != 0) {
        // path not exist
        return NULL;
    }
    struct gitmarket *ret = (struct gitmarket *) malloc (sizeof (*ret));
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
    int basepath_len = strlen (basepath);
    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_DIR && ent->d_name[0] != '.') {
            struct gitrepo *repo = (struct gitrepo *) malloc (sizeof (*repo));
            if (repo == NULL) {
                gitmarket_dispose (ret);
                return NULL;
            }
            repo->name = strdup (ent->d_name);
            if (repo->name == NULL) {
                free (repo);
                gitmarket_dispose (ret);
                return NULL;
            }
            int reponame_len = strlen (ent->d_name);
            repo->path = __gitrepo_get_path (basepath, basepath_len, ent->d_name, reponame_len);
            if (repo->path == NULL) {
                free (repo->name);
                free (repo);
                gitmarket_dispose (ret);
                return NULL;
            }

            __gitmarket_append (ret, repo);
        }
    }

    return ret;
}

void gitmarket_dispose (struct gitmarket *market) {
    struct gitrepo *repo = market->head;

    while (repo) {
        free (repo->path);
        free (repo->name);

        struct gitrepo *next_repo = repo->next;
        free (repo);
        repo = next_repo;
    }

    free (market->path);
    free (market);
}

void gitmarket_reset (struct gitmarket *market) {
    if (market == NULL) {
        DBG_LOG (DBG_ERROR, "gitmarket_reset: market is null");
        return;
    }

    market->cursor = NULL;
}

int gitmarket_hasnext (struct gitmarket *market) {
    if (market == NULL) {
        DBG_LOG (DBG_ERROR, "gitmarket_hasnext: market is null");
        return 0;
    }

    return (market->cursor == NULL ?market->head : market->cursor->next) != NULL;
}

struct gitrepo *gitmarket_next (struct gitmarket *market) {
    if (market == NULL) {
        DBG_LOG (DBG_ERROR, "gitmarket_next: market is null");
        return NULL;
    }

    return (market->cursor = (market->cursor == NULL ? market->head : market->cursor->next));
}

G_KID_EXTERN char *gitrepo_get_path (struct gitrepo *repo) {
    if (repo == NULL) {
        return NULL;
    }
    return repo->path;
}

G_KID_EXTERN char *gitrepo_get_name (struct gitrepo *repo) {
    if (repo == NULL) {
        return NULL;
    }
    return repo->name;
}