#include <stdlib.h>

#ifndef __AEON_BTREE_H_
#define __AEON_BTREE_H_

typedef struct _aeon_btree_node
{
    int* values;
    int value_count;
    int leaf;
    struct _aeon_btree_node **children;
    struct _aeon_btree_node *parent;
} aeon_btree_node;

typedef struct _aeon_btree
{
    struct _aeon_btree_node *root;
    int order;
} aeon_btree;

aeon_btree_node *aeon_btree_node_create(aeon_btree *tree);

aeon_btree *aeon_btree_create(int order);

void aeon_btree_split(aeon_btree *_tree, aeon_btree_node *_parent,
        aeon_btree_node *_node, int median_index);

void aeon_btree_node_insert(aeon_btree *_tree, aeon_btree_node *_node,
        int value);

void aeon_btree_insert(aeon_btree *_tree, int value);

#endif
