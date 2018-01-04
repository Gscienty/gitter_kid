#ifndef _G_KID_RDTREE_
#define _G_KID_RDTREE_

#include "define.h"

enum rdt_color {
    RDTREE_COLOR_RED,
    RDTREE_COLOR_BLACK
};

struct rdt_node {
    void *key;
    int val;
    enum rdt_color color;

    struct rdt_node *parent;
    struct rdt_node *left;
    struct rdt_node *right;
};

struct rdt {
    struct rdt_node *root;

    struct rdt_node nil;
};

struct rdt *rdt_build ();
void rdt_insert (struct rdt *tree, void *key, int val);
struct rdt_node *rdt_find (struct rdt *tree, const char *key);
void rdt_dispose (struct rdt *tree);

void __rdt_left_rotate (struct rdt *tree, struct rdt_node *n);
void __rdt_right_rotate (struct rdt *tree, struct rdt_node *n);
void __rdt_insert_fix (struct rdt *tree, struct rdt_node *n);
int __compare_byte_string (const void *a, const void *b);
int __compare_char_string (const char *a, const void *b);


#endif