#include "gitfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

struct git_person_log *__transfer_person_log (const char *ch) {
        char *fields[4];
        register int i;
        for (i = 0; ch && i < 4; i++) {
            fields[i] = (char *) ch;
            char *space_ptr = strchr (ch, ' ');
            if (space_ptr != NULL) {
                *space_ptr = 0;
                ch = space_ptr + 1;
            }
            else {
                ch = NULL;
            }
        }
        if (i != 4) {
            //author format error
            DBG_LOG (DBG_ERROR, "author format error");
            return NULL;
        }

        struct git_person_log *ret = (struct git_person_log *) malloc (sizeof (*ret));
        ret->name = fields[0];
        ret->mail = fields[1];
        char *ep;
        if ((ret->timestamp = strtol (fields[2], &ep, 10)) == 0 && *ep) {
            // timestamp error;
            DBG_LOG(DBG_ERROR, "timestamp format error");
            free (ret);
            return NULL;
        }

        ret->timezone = fields[3];

        return ret;
}

struct git_obj_commit *__git_obj_transfer_commit (struct git_obj *obj) {
    struct git_obj_commit *ret = (struct git_obj_commit *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // have not enough free memory
        DBG_LOG (DBG_ERROR, "git_obj_get_commit: have not enough free memory");
        return NULL;
    }
    ret->parent_head = ret->parent_tail = ret->parent_cursor = NULL;
    ret->tree_sign = ret->message = NULL;
    ret->author = ret->committer = NULL;

    register char *ch;
    for (ch = obj->body; *ch;) {
        char *end_ptr = strchr (ch, '\n');
        if (end_ptr == NULL) {
            break;
        }
        *end_ptr = 0;
        if (end_ptr == ch) {
            ret->message = ch + 1;
            break;
        }
        char *space_ptr = strchr (ch, ' ');
        *space_ptr = 0;
        if (strcmp (ch, "tree") == 0) {
            ch = space_ptr + 1;
            ret->tree_sign = ch;
        }
        else if (strcmp (ch, "author") == 0) {
            struct git_person_log *author = __transfer_person_log (space_ptr + 1);
            if (author == NULL) {
                // cannnot transfer author
                DBG_LOG (DBG_ERROR, "git_obj_get_commit: author format error");
                __git_obj_commit_dispose (ret);
                return NULL;
            }
            ret->author = author;
        }
        else if (strcmp (ch, "committer") == 0) {
            struct git_person_log *committer = __transfer_person_log (space_ptr + 1);
            if (committer == NULL) {
                // cannnot transfer committer
                DBG_LOG (DBG_ERROR, "git_obj_get_commit: committer format error");
                __git_obj_commit_dispose (ret);
                return NULL;
            }
            ret->committer = committer;
        }
        else if (strcmp (ch, "parent") == 0) {
            ch = space_ptr + 1;
            struct git_obj_commit_parent *parent = (struct git_obj_commit_parent *) malloc (sizeof (*parent));
            if (parent == NULL) {
                // not enough free memory
                DBG_LOG (DBG_ERROR, "git_obj_get_commit: not enough free memory");
                __git_obj_commit_dispose (ret);
                return NULL;
            }
            parent->sign = ch;
            parent->prev = ret->parent_tail;
            parent->next = NULL;
            if (ret->parent_head == NULL) {
                ret->parent_head = parent;
            }
            if (ret->parent_tail != NULL) {
                ret->parent_tail->next = parent;
            }
            ret->parent_tail = parent;
        }

        ch = end_ptr + 1;
    }
    return ret;
}

void __git_obj_commit_dispose (struct git_obj_commit *obj) {
    if (obj == NULL) {
        return ;
    }

    if (obj->author != NULL) {
        free (obj->author);
    }
    if (obj->committer != NULL) {
        free (obj->committer);
    }
    if (obj->parent_head != NULL) {
        while (obj->parent_head) {
            struct git_obj_commit_parent *next = obj->parent_head->next;
            free (obj->parent_head);
            obj->parent_head = next;
        }
    }

    free (obj);
}

struct git_obj_commit *git_obj_get_commit (struct git_obj *obj) {
    if (obj == NULL) {
        return NULL;
    }

    if (obj->type == GIT_OBJ_TYPE_COMMIT) {
        return (struct git_obj_commit *) obj->ptr;
    }
    else {
        return NULL;
    }
}