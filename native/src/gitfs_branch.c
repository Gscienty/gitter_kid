#include "gitfs.h"
#include <malloc.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

struct git_branches *git_branches_get (struct git_repo *repo) {
    if (repo == NULL) {
        DBG_LOG (DBG_ERROR, "git_branches_get: repo is null");
        return NULL;
    }
    struct git_branches *branches = (struct git_branches *) malloc (sizeof (*branches));
    if (branches == NULL) {
        DBG_LOG (DBG_ERROR, "git_branches_get: have not enough free memory");
        return NULL;
    }
    branches->head = branches->tail = branches->cursor = NULL;

    int basepath_length = strlen (repo->path);
    char *refs_path = (char *) malloc (sizeof (char) * (basepath_length + 12));
    if (refs_path == NULL) {
        DBG_LOG (DBG_ERROR, "git_branches_get: have not enough free memory");
        free (branches);
        return NULL;
    }
    strcpy (refs_path, repo->path);
    strcpy (refs_path + basepath_length, "refs/heads/");
    
    DIR *dir = opendir (refs_path);
    if (dir == NULL) {
        DBG_LOG (DBG_ERROR, "git_branches_get: cannnot open dir");
        free (branches);
        free (refs_path);
        return NULL;
    }

    struct dirent *ent = NULL;
    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_REG) {
            struct git_branch *branch = (struct git_branch *) malloc (sizeof (*branch));
            if (branch == NULL) {
                DBG_LOG (DBG_ERROR, "git_branches_get: have not enough free memory");
                free (refs_path);
                git_branches_dispose (branches);
                return NULL;
            }

            branch->name = strdup (ent->d_name);
            if (branch->name == NULL) {
                DBG_LOG (DBG_ERROR, "git_branches_get: have not enough free memory");
                free (branch);
                free (refs_path);
                git_branches_dispose (branches);
                return NULL;
            }

            int name_length = strlen (branch->name);
            char *ref_path = (char *) malloc (sizeof (char) * (basepath_length + 12 + name_length));
            if (ref_path == NULL) {
                DBG_LOG (DBG_ERROR, "git_branches_get: have not enough free memory");
                free (branch->name);
                free (branch);
                free (refs_path);
                git_branches_dispose (branches);
                return NULL;
            }
            strcpy (ref_path, refs_path);
            strcpy (ref_path + basepath_length + 11, branch->name);
            ref_path[basepath_length + 11 + name_length] = 0;

            branch->last_commit_sign = (char *) malloc (sizeof (char) * 41);
            if (branch->last_commit_sign == NULL) {
                DBG_LOG (DBG_ERROR, "git_branches_get: have not enough free memory");
                free (branch->name);
                free (branch);
                free (ref_path);
                free (refs_path);
                git_branches_dispose (branches);
            }
            FILE *head = fopen (ref_path, "r");
            fread(branch->last_commit_sign, sizeof (char), 40, head);
            fclose (head);
            free (ref_path);

            branch->prev = branches->tail;
            branch->next = NULL;
            if (branches->head == NULL) {
                branches->head = branch;
            }
            if (branches->tail) {
                branches->tail->next = branch;
            }
            branches->tail = branch;
        }
    }
    free (refs_path);
    closedir (dir);
    return branches;
}

void git_branches_dispose (struct git_branches *branches) {
    if (branches == NULL) {
        return;
    }

    while (branches->head) {
        struct git_branch *next_branch = branches->head->next;
        free (branches->head->name);
        free (branches->head->last_commit_sign);
        free (branches->head);
        branches->head = next_branch;
    }
}

void git_branches_reset (struct git_branches *branches) {
    if (branches == NULL) {
        return;
    }
    branches->cursor = branches->head;
}

int git_branches_move_next (struct git_branches *branches) {
    if (branches == NULL) {
        DBG_LOG (DBG_ERROR, "git_branches_move_next: branches is null");
        return -1;
    }
    if (branches->cursor == NULL) {
        DBG_LOG (DBG_ERROR, "git_branches_move_next: branches's cursor is null");
    }

    return (branches->cursor = branches->cursor->next) == NULL ? -1 : 0;
}

struct git_branch *git_branches_get_current (struct git_branches *branches) {
    if (branches == NULL) {
        DBG_LOG (DBG_ERROR, "git_branches_get_current: branches is null");
        return NULL;
    }
    return branches->cursor;
}

char *git_branch_get_name (struct git_branch *branch) {
    if (branch == NULL) {
        DBG_LOG (DBG_ERROR, "git_branch_get_name: branch is null");
        return NULL;
    }
    return branch->name;
}

char *git_branch_get_last_commit_sign (struct git_branch *branch) {
    if (branch == NULL) {
        DBG_LOG (DBG_ERROR, "git_branch_get_last_commit_sign: branch is null");
        return NULL;
    }
    return branch->last_commit_sign;
}