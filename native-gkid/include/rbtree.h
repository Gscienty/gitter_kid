#ifndef _G_KID_RDTREE_
#define _G_KID_RDTREE_

#include "define.h"

enum rdt_color {
    RDTREE_COLOR_RED,
    RDTREE_COLOR_BLACK
};

struct rdt_node {
    void *key;
    int off;
    int len;
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
void rdt_insert (struct rdt *tree, void *key, int offset, int len);
struct rdt_node *rdt_find__char_string (const struct rdt *tree, const void *key);
struct rdt_node *rdt_find__byte_string (const struct rdt *tree, const void *key);
void rdt_dtor (struct rdt *tree);


#endif