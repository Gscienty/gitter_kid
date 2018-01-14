#include "gitfs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

struct gitobj_tree *__git_obj_transfer_tree (char *body, size_t size) {
    struct gitobj_tree *ret = (struct gitobj_tree *) malloc (sizeof (*ret));
    if (ret == NULL) {
        // have not enough free memory
        DBG_LOG (DBG_ERROR, "git_obj_get_tree: have not enough free memroy");
        return NULL;
    }
    ret->head = ret->tail = ret->cursor = NULL;

    register char *ch = body;
    while (ch < body + size) {
        struct gitobj_treeitem *tree_item = (struct gitobj_treeitem *) malloc (sizeof (*tree_item));
        if (tree_item == NULL) {
            DBG_LOG (DBG_ERROR, "git_obj_get_tree: have not enough free memory");
            __gitobj_tree_dispose (ret);
            return NULL;
        }

        char *space_ptr = strchr (ch, ' ');
        *space_ptr = 0;

        char *ep;
        if ((tree_item->type = strtol (ch, &ep, 10)) == 0 && *ep) {
            DBG_LOG (DBG_ERROR, "git_obj_get_tree: type format error");
            __gitobj_tree_dispose (ret);
            return NULL;
        }
        tree_item->name = ch = space_ptr + 1;
        ch += strlen (ch) + 1;

        // should know sign need free.
        tree_item->sign = (char *) malloc (sizeof (char) * 41);
        if (tree_item->sign == NULL) {
            DBG_LOG (DBG_ERROR, "git_obj_get_tree: have not enough free memory");
            free (tree_item);
            __gitobj_tree_dispose (ret);
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

void __gitobj_tree_dispose (struct gitobj_tree *obj) {
    if (obj == NULL) {
        return ;
    }
    while (obj->head != NULL) {
        struct gitobj_treeitem *next = obj->head->next;
        
        if (obj->head->sign != NULL) {
            free (obj->head->sign);
        }
        free (obj->head);
        obj->head = next;
    }
    free (obj);
}

struct gitobj_tree *git_obj_get_tree (struct gitobj *obj) {
    if (obj == NULL) {
        return NULL;
    }

    if (obj->type == GIT_OBJ_TYPE_TREE) {
        return (struct gitobj_tree *) obj->ptr;
    }
    else {
        return NULL;
    }
}

void gitobj_tree_reset (struct gitobj_tree *tree_obj) {
    if (tree_obj == NULL) {
        DBG_LOG (DBG_ERROR, "gitobj_tree_reset: tree object is null");
        return ;
    }
    tree_obj->cursor = tree_obj->head;
}

int gitobj_tree_move_next (struct gitobj_tree *tree_obj) {
    if (tree_obj == NULL) {
        DBG_LOG (DBG_ERROR, "gitobj_tree_move_next: tree object is null");
        return -1;
    }
    if (tree_obj->cursor == NULL) {
        DBG_LOG (DBG_INFO, "gitobj_tree_move_next: neet reset");
        return -2;
    }
    tree_obj->cursor = tree_obj->cursor->next;
    if (tree_obj->cursor == NULL) {
        DBG_LOG (DBG_INFO, "gitobj_tree_move_next: not have next entry");
        return -3;
    }
    return 0;
}

struct gitobj_treeitem *gitobj_tree_current (struct gitobj_tree *tree_obj) {
    if (tree_obj == NULL) {
        DBG_LOG (DBG_ERROR, "gitobj_tree_current: tree object is null");
        return NULL;
    }
    return tree_obj->cursor;
}

char *gitobj_treeitem_sign (struct gitobj_treeitem *tree_item_obj) {
    if (tree_item_obj == NULL) {
        DBG_LOG (DBG_ERROR, "gitobj_treeitem_sign: tree item object is null");
        return NULL;
    }
    return tree_item_obj->sign;
}

char *gitobj_treeitem_name (struct gitobj_treeitem *tree_item_obj) {
    if (tree_item_obj == NULL) {
        DBG_LOG (DBG_ERROR, "gitobj_treeitem_name: tree item object is null");
        return NULL;
    }
    return tree_item_obj->name;
}

enum git_obj_type gitobj_treeitem_type (struct gitobj_treeitem *tree_item_obj) {
    if (tree_item_obj == NULL) {
        DBG_LOG (DBG_ERROR, "gitobj_treeitem_name: tree item object is null");
        return GIT_OBJ_TYPE_UNKNOW;
    }
    return tree_item_obj->type == 40000 ? GIT_OBJ_TYPE_TREE : GIT_OBJ_TYPE_BLOB;
}