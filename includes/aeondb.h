#ifndef __AEONDB_H_
#define __AEONDB_H_

void *aeon_initialise(char *_location, int location_length);
void *aeon_tag_create(void *_dbhandle, char *_name, int name_length);
void aeon_tag_save(void *_tag);

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

aeon_btree *aeon_btree_create(int order);
void aeon_btree_insert(aeon_btree *_tree, int value);

#endif
