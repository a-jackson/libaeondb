#include <stdlib.h>

#ifndef __AEON_BTREE_H_
#define __AEON_BTREE_H_

#define TREE_MAGIC_BYTE 0x0A
#define KEY_TYPE unsigned long
#define VALUE_TYPE unsigned long

typedef struct _aeon_btree_header
{
    char magic_byte;
    unsigned int order;
    unsigned int node_size;
    unsigned long root_position;
} aeon_btree_header;

typedef struct _aeon_btree_node
{
    KEY_TYPE* keys;
    VALUE_TYPE *values;
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
    char *file_name;
    FILE *file;
} aeon_btree;

aeon_btree_node *aeon_btree_node_create(aeon_btree *tree);

aeon_btree *aeon_btree_create(int order, char *_file, int file_length);

void aeon_btree_split(aeon_btree *_tree, aeon_btree_node *_parent,
        aeon_btree_node *_node, int median_index);

void aeon_btree_node_insert(aeon_btree *_tree, aeon_btree_node *_node,
        KEY_TYPE key, VALUE_TYPE value);

void aeon_btree_insert(aeon_btree *_tree, KEY_TYPE key, VALUE_TYPE value);
void aeon_btree_save(aeon_btree *_tree, int header_only);
void aeon_btree_node_load(aeon_btree *_tree, aeon_btree_node *_node);
aeon_btree *aeon_btree_load(char *_file, int file_length);
void aeon_btree_node_update(aeon_btree *_tree, aeon_btree_node *_node);
void aeon_btree_node_save(aeon_btree *_tree, aeon_btree_node *_node);
void aeon_btree_free(aeon_btree *_tree);
void aeon_btree_node_free(aeon_btree_node *_node);
void aeon_btree_save_new_node(aeon_btree *_tree, aeon_btree_node *_node);

void aeon_btree_open(aeon_btree *_tree, int new_file);
void aeon_btree_close(aeon_btree *_tree);
int aeon_btree_node_get_value(aeon_btree *_tree, aeon_btree_node *_node,
        KEY_TYPE key, VALUE_TYPE *value);
int aeon_btree_get_value(aeon_btree *_tree, KEY_TYPE key, VALUE_TYPE *value);
aeon_btree *aeon_btree_initialise(int order, char *_file, int file_length);

#endif
