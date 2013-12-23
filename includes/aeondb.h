#ifndef __AEONDB_H_
#define __AEONDB_H_

void *aeon_initialise(char *_location, int location_length);
void *aeon_tag_create(void *_dbhandle, char *_name, int name_length);
void aeon_tag_save(void *_tag);

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
} aeon_btree;

aeon_btree *aeon_btree_create(int order);
void aeon_btree_insert(aeon_btree *_tree, int value);

#endif
