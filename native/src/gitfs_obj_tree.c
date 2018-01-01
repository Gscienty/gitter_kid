#include "gitfs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

struct git_obj_tree *__git_obj_transfer_tree (struct git_obj *obj) {
    struct git_obj_tree *ret = (struct git_obj_tree *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // have not enough free memory
        DBG_LOG (DBG_ERROR, "git_obj_get_tree: have not enough free memroy");
        return NULL;
    }
    ret->head = ret->tail = ret->cursor = NULL;

    register char *ch = obj->body;
    while (ch < obj->body + obj->size) {
        struct git_obj_tree_item *tree_item = (struct git_obj_tree_item *) malloc (sizeof (*tree_item));
        if (tree_item == NULL) {
            DBG_LOG (DBG_ERROR, "git_obj_get_tree: have not enough free memory");
            __git_obj_tree_dispose (ret);
            return NULL;
        }

        char *space_ptr = strchr (ch, ' ');
        *space_ptr = 0;

        char *ep;
        if ((tree_item->type = strtol (ch, &ep, 10)) == 0 && *ep) {
            DBG_LOG (DBG_ERROR, "git_obj_get_tree: type format error");
            __git_obj_tree_dispose (ret);
            return NULL;
        }
        tree_item->name = ch = space_ptr + 1;
        ch += strlen (ch) + 1;

        // should know sign need free.
        tree_item->sign = (char *) malloc (sizeof (char) * 41);
        if (tree_item->sign == NULL) {
            DBG_LOG (DBG_ERROR, "git_obj_get_tree: have not enough free memory");
            free (tree_item);
            __git_obj_tree_dispose (ret);
            return NULL;
        }

        register int i;
        for (i = 0; i < 20; i++) {
            sprintf(tree_item->sign + (i << 1), "%02x", (unsigned char)ch[i]);
        }
        tree_item->sign[40] = 0;
        ch += 20;
        
        tree_item->prev = ret->tail;
        tree_item->next = NULL;
        if (ret->head == NULL) {
            ret->head = tree_item;
        }
        if (ret->tail != NULL) {
            ret->tail->next = tree_item;
        }
        ret->tail = tree_item;
    }
    return ret;
}

void __git_obj_tree_dispose (struct git_obj_tree *obj) {
    if (obj == NULL) {
        return ;
    }
    while (obj->head != NULL) {
        struct git_obj_tree_item *next = obj->head->next;
        
        if (obj->head->sign != NULL) {
            free (obj->head->sign);
        }
        free (obj->head);
        obj->head = next;
    }
    free (obj);
}

struct git_obj_tree *git_obj_get_tree (struct git_obj *obj) {
    if (obj == NULL) {
        return NULL;
    }

    if (obj->type == GIT_OBJ_TYPE_TREE) {
        return (struct git_obj_tree *) obj->ptr;
    }
    else {
        return NULL;
    }
}

void git_obj_tree_reset (struct git_obj_tree *tree_obj) {
    if (tree_obj == NULL) {
        DBG_LOG (DBG_ERROR, "git_obj_tree_reset: tree object is null");
        return ;
    }
    tree_obj->cursor = tree_obj->head;
}

int git_obj_tree_move_next (struct git_obj_tree *tree_obj) {
    if (tree_obj == NULL) {
        DBG_LOG (DBG_ERROR, "git_obj_tree_move_next: tree object is null");
        return -1;
    }
    if (tree_obj->cursor == NULL) {
        DBG_LOG (DBG_INFO, "git_obj_tree_move_next: neet reset");
        return -2;
    }
    tree_obj->cursor = tree_obj->cursor->next;
    if (tree_obj->cursor == NULL) {
        DBG_LOG (DBG_INFO, "git_obj_tree_move_next: not have next entry");
        return -3;
    }
    return 0;
}

struct git_obj_tree_item *git_obj_tree_current (struct git_obj_tree *tree_obj) {
    if (tree_obj == NULL) {
        DBG_LOG (DBG_ERROR, "git_obj_tree_current: tree object is null");
        return NULL;
    }
    return tree_obj->cursor;
}

char *git_obj_tree_item_sign (struct git_obj_tree_item *tree_item_obj) {
    if (tree_item_obj == NULL) {
        DBG_LOG (DBG_ERROR, "git_obj_tree_item_sign: tree item object is null");
        return NULL;
    }
    return tree_item_obj->sign;
}

char *git_obj_tree_item_name (struct git_obj_tree_item *tree_item_obj) {
    if (tree_item_obj == NULL) {
        DBG_LOG (DBG_ERROR, "git_obj_tree_item_name: tree item object is null");
        return NULL;
    }
    return tree_item_obj->name;
}

enum git_obj_type git_obj_tree_item_type (struct git_obj_tree_item *tree_item_obj) {
    if (tree_item_obj == NULL) {
        DBG_LOG (DBG_ERROR, "git_obj_tree_item_name: tree item object is null");
        return GIT_OBJ_TYPE_UNKNOW;
    }
    return tree_item_obj->type == 40000 ? GIT_OBJ_TYPE_TREE : GIT_OBJ_TYPE_BLOB;
}