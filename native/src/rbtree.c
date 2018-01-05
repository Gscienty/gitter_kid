#include "rbtree.h"
#include <malloc.h>

struct rdt *rdt_build () {
    struct rdt *result = (struct rdt *) malloc (sizeof (*result));
    if (result == NULL) {
        DBG_LOG (DBG_ERROR, "rdt_build: have not enough free memory");
        return NULL;
    }
    result->root = &result->nil;
    result->nil.color = RDTREE_COLOR_BLACK;
    return result;
}

void rdt_insert (struct rdt *tree, void *key, int offset, int len) {
    if (tree == NULL) {
        DBG_LOG (DBG_ERROR, "rdt_insert: tree is null");
        return;
    }

    struct rdt_node *n = (struct rdt_node *) malloc (sizeof (*n));
    if (n == NULL) {
        DBG_LOG (DBG_ERROR, "rdt_insert: have not enough free memory");
        return;
    }
    n->key = key;
    n->offset = offset;
    n->len = len;
    n->parent = n->left = n->right = &tree->nil;
    n->color = RDTREE_COLOR_RED;

    struct rdt_node *x = tree->root;
    struct rdt_node *y = &tree->nil;

    while (x != &tree->nil) {
        y = x;

        if (__compare_byte_string (n->key, x->key) == -1) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    n->parent = y;

    if (y == &tree->nil) {
        tree->root = n;
    }
    else if (__compare_byte_string (n->key, y->key) == -1) {
        y->left = n;
    }
    else {
        y->right = n;
    }

    __rdt_insert_fix (tree, n);
}

void __rdt_left_rotate (struct rdt *tree, struct rdt_node *n) {
    struct rdt_node *m = n->right;
    n->right = m->left;
    if (m->left != &tree->nil) {
        m->left->parent = n;
    }
    m->parent = n->parent;
    if (n->parent == &tree->nil) {
        tree->root = m;
    }
    else if (n == n->parent->left) {
        n->parent->left = m;
    }
    else {
        n->parent->right = m;
    }
    m->left = n;
    n->parent = m;
}


void __rdt_right_rotate (struct rdt *tree, struct rdt_node *n) {
    struct rdt_node *m = n->left;
    n->left = m->right;
    if (m->right != &tree->nil) {
        m->right->parent = n;
    }
    m->parent = n->parent;
    if (n->parent == &tree->nil) {
        tree->root = m;
    }
    else if (n == n->parent->left) {
        n->parent->left = m;
    }
    else {
        n->parent->right = m;
    }

    m->right = n;
    n->parent = m;
}

void __rdt_insert_fix (struct rdt *tree, struct rdt_node *n) {
    while (n->parent->color == RDTREE_COLOR_RED) {
        if (n->parent == n->parent->parent->left) {
            struct rdt_node *uncle = n->parent->parent->right;

            if (uncle->color == RDTREE_COLOR_RED) {
                n->parent->color = RDTREE_COLOR_BLACK;
                uncle->color = RDTREE_COLOR_BLACK;
                n->parent->parent->color = RDTREE_COLOR_RED;
                n = n->parent->parent;
            }
            else {
                if (n == n->parent->right) {
                    n = n->parent;
                    __rdt_left_rotate (tree, n);
                }
                n->parent->color = RDTREE_COLOR_BLACK;
                n->parent->parent->color = RDTREE_COLOR_RED;
                __rdt_right_rotate (tree, n->parent->parent);
            }
        }
        else {
            struct rdt_node *uncle = n->parent->parent->left;

            if (uncle->color == RDTREE_COLOR_RED) {
                n->parent->color = RDTREE_COLOR_BLACK;
                uncle->color = RDTREE_COLOR_BLACK;
                n->parent->parent->color = RDTREE_COLOR_RED;
                n = n->parent->parent;
            }
            else {
                if (n == n->parent->left) {
                    n = n->parent;
                    __rdt_right_rotate (tree, n);
                }
                n->parent->color = RDTREE_COLOR_BLACK;
                n->parent->parent->color = RDTREE_COLOR_RED;
                __rdt_left_rotate (tree, n->parent->parent);
            }
        }
    }
    tree->root->color = RDTREE_COLOR_BLACK;
}

int __compare_byte_string (const void *a, const void *b) {
    unsigned char *vala = (unsigned char *) a;
    unsigned char *valb = (unsigned char *) b;

    int i;
    for (i = 0; i < 20; i++) {
        if (vala[i] < valb[i]) {
            return -1;
        }
        else if (vala[i] > valb[i]) {
            return 1;
        }
    }

    return 0;
}

unsigned char __transfer_hex_to_harf_byte (char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    else {
        return c - 'a' + 10;
    }
}

unsigned char __transfer_hex_to_byte (const char *str, int offset) {
    return (__transfer_hex_to_harf_byte (str[offset << 1]) << 4)
        | (__transfer_hex_to_harf_byte (str[(offset << 1) + 1]));
}

int __compare_char_string (const char *a, const void *b) {
    unsigned char *valb = (unsigned char *) b;

    int i;
    for (i = 0; i < 20; i++) {
        unsigned char valai = __transfer_hex_to_byte (a, i);
        if (valai < valb[i]) {
            return -1;
        }
        else if (valai > valb[i]) {
            return 1;
        }
    }

    return 0;
}

struct rdt_node *rdt_find (struct rdt *tree, const char *key) {
    struct rdt_node *retnode = tree->root;
    while (retnode != &tree->nil) {
        int compare_result = __compare_char_string (key, retnode->key);
        if (compare_result == 0) {
            return retnode;
        }
        else if (compare_result == -1) {
            retnode = retnode->left;
        }
        else {
            retnode = retnode->right;
        }
    }

    return NULL;
}

void rdt_dispose (struct rdt *tree) {
    // TODO
}