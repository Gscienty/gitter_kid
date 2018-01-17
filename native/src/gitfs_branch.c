#include "gitfs.h"
#include <malloc.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

DIR *__gitrepo_get_branchesdir (const char *repopath, const size_t len) {
    char *refs_path = (char *) malloc (sizeof (char) * (len + 12));
    if (refs_path == NULL) {
        DBG_LOG (DBG_ERROR, "__gitrepo_get_branchesdir: have not enough free memory");
        return NULL;
    }
    strcpy (refs_path, repopath);
    strcpy (refs_path + len, "refs/heads/");
    
    DIR *ret = opendir (refs_path);
    free (refs_path);

    return ret;
}

char *__gitbranch_get_branchpath (const char *repopath, size_t repopath_len, const char *branchname, size_t branchname_len) {
    
    char *ret = (char *) malloc (sizeof (char) * (repopath_len + 12 + branchname_len));
    if (ret == NULL) {
        DBG_LOG (DBG_ERROR, "__gitbranch_get_branchpath: have not enough free memory");
        return NULL;
    }
    
    strcpy (ret, repopath);
    strcpy (ret + repopath_len, "refs/heads/");
    strcpy (ret + repopath_len + 11, branchname);

    return ret;
}

void __gitbranches_append (struct gitbranches *branches, struct gitbranch *branch) {
    if (branches == NULL || branch == NULL) return;
    
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

struct gitbranches *gitrepo_get_branches (struct gitrepo *repo) {
    if (repo == NULL) {
        DBG_LOG (DBG_ERROR, "gitrepo_get_branches: repo is null");
        return NULL;
    }

    size_t repopath_len = sizeof (repo->path);

    // get branches's path
    DIR *dir = __gitrepo_get_branchesdir (repo->path, repopath_len);
    if (dir == NULL) {
        DBG_LOG (DBG_ERROR, "gitrepo_get_branches: cannnot open dir");
        return NULL;
    }

    // init branches collection
    struct gitbranches *branches = (struct gitbranches *) malloc (sizeof (*branches));
    if (branches == NULL) {
        DBG_LOG (DBG_ERROR, "gitrepo_get_branches: have not enough free memory");
        return NULL;
    }
    branches->head = branches->tail = branches->cursor = NULL;

    struct dirent *ent = NULL;
    while ((ent = readdir (dir))) {
        if (ent->d_type == DT_REG) {
            struct gitbranch *branch = (struct gitbranch *) malloc (sizeof (*branch));
            if (branch == NULL) {
                DBG_LOG (DBG_ERROR, "gitrepo_get_branches: have not enough free memory");
                gitbranches_dtor (branches);
                return NULL;
            }

            branch->name = strdup (ent->d_name);
            if (branch->name == NULL) {
                DBG_LOG (DBG_ERROR, "git_brangitrepo_get_branchesches_get: have not enough free memory");
                free (branch);
                gitbranches_dtor (branches);
                return NULL;
            }

            char *ref_path = __gitbranch_get_branchpath (repo->path, repopath_len, branch->name, strlen (branch->name));
            branch->lastcommit_sign = (char *) malloc (sizeof (char) * 41);
            if (ref_path == NULL || branch->lastcommit_sign == NULL) {
                DBG_LOG (DBG_ERROR, "gitrepo_get_branches: have not enough free memory");
                free (branch->name);
                free (branch);
                gitbranches_dtor (branches);
            }
            
            FILE *head = fopen (ref_path, "r");
            fread(branch->lastcommit_sign, sizeof (char), 40, head);
            branch->lastcommit_sign[40] = 0;
            fclose (head);
            free (ref_path);

            __gitbranches_append (branches, branch);
        }
    }
    closedir (dir);
    return branches;
}

void gitbranches_dtor (struct gitbranches *branches) {
    if (branches == NULL) {
        return;
    }

    while (branches->head) {
        struct gitbranch *next_branch = branches->head->next;
        free (branches->head->name);
        free (branches->head->lastcommit_sign);
        free (branches->head);
        branches->head = next_branch;
    }
}

char *gitbranch_get_name (struct gitbranch *branch) {
    if (branch == NULL) {
        DBG_LOG (DBG_ERROR, "gitbranch_get_name: branch is null");
        return NULL;
    }
    return branch->lastcommit_sign;
}

char *gitbranch_get_lastcommit_sign (struct gitbranch *branch) {
    if (branch == NULL) {
        DBG_LOG (DBG_ERROR, "gitbranch_get_lastcommit_sign: branch is null");
        return NULL;
    }
    return branch->lastcommit_sign;
}

void gitbranches_reset (struct gitbranches *branches) {
    if (branches == NULL) {
        DBG_LOG (DBG_ERROR, "gitbranches_reset: branches is null");
        return;
    }

    branches->cursor = NULL;
}

int gitbranches_hasnext (struct gitbranches *branches) {
    if (branches == NULL) {
        DBG_LOG (DBG_ERROR, "gitbranches_reset: branches is null");
        return 0;
    }

    return (branches->cursor == NULL ? branches->head : branches->cursor->next) != NULL;
}

struct gitbranch *gitbranches_next (struct gitbranches *branches) {
    if (branches == NULL) {
        DBG_LOG (DBG_ERROR, "gitbranches_reset: branches is null");
        return NULL;
    }

    return (branches->cursor = (branches->cursor == NULL ? branches->head : branches->cursor->next));
}