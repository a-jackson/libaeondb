#include <stdlib.h>

#ifndef __AEON_BTREE_H_
#define __AEON_BTREE_H_

#define TREE_MAGIC_BYTE 0x0A

typedef struct _aeon_btree_header
{
    char magic_byte;
    unsigned int order;
    unsigned int node_size;
    unsigned long root_position;
} aeon_btree_header;

typedef struct _aeon_btree_node
{
    unsigned long* keys;
    unsigned int value_count;
    unsigned int leaf;
    struct _aeon_btree_node **children;
    unsigned long position;
    unsigned long *children_positions;
} aeon_btree_node;

typedef struct _aeon_btree
{
    struct _aeon_btree_node *root;
    unsigned int order;
    unsigned int node_size;
    char *file;
} aeon_btree;

aeon_btree_node *aeon_btree_node_create(aeon_btree *tree);

aeon_btree *aeon_btree_create(int order, char *_file, int file_length);

void aeon_btree_split(aeon_btree *_tree, aeon_btree_node *_parent,
        aeon_btree_node *_node, int median_index);

void aeon_btree_node_insert(aeon_btree *_tree, aeon_btree_node *_node,
        int value);

void aeon_btree_insert(aeon_btree *_tree, int value);
void aeon_btree_save(aeon_btree *_tree, int header_only);
void aeon_btree_node_load(aeon_btree *_tree, aeon_btree_node *_node, FILE *file);
aeon_btree *aeon_btree_load(char *_file, int file_length);
void aeon_btree_node_update(aeon_btree *_tree,
        aeon_btree_node *_node);
void aeon_btree_node_save(FILE *file, aeon_btree *_tree, aeon_btree_node *_node);
void aeon_btree_free(aeon_btree *_tree);
void aeon_btree_node_free(aeon_btree_node *_node);
void aeon_btree_save_new_node(aeon_btree *_tree, aeon_btree_node *_node);

#endif
