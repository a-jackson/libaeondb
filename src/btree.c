#include <stdlib.h>
#include <stdio.h>
#include "btree.h"


aeon_btree_node *aeon_btree_node_create(aeon_btree *tree)
{
    aeon_btree_node *node;

    node = malloc(sizeof(aeon_btree_node));

    node->keys = malloc(sizeof(unsigned long) * ((2 * tree->order) - 1));
    node->value_count = 0;
    node->leaf = 1;
    node->children = calloc(2 * tree->order, sizeof(aeon_btree_node));
    node->children_positions = malloc(sizeof(unsigned long) * 2 * tree->order);

    return node;
}

aeon_btree *aeon_btree_create(int order)
{
    aeon_btree *tree;
    unsigned int node_size;

    tree = malloc(sizeof(aeon_btree));

    tree->order = order;
    tree->root = aeon_btree_node_create(tree);

    node_size = sizeof(unsigned long) * ((2 * order) - 1); // keys
    node_size += sizeof(unsigned int) * 2; // value_count & leaf
    node_size += sizeof(unsigned long) * 2 * order; // children_positions
    tree->node_size = node_size;

    return tree;
}

void aeon_btree_node_save(FILE *file, aeon_btree *_tree, aeon_btree_node *_node,
        int update)
{
    fpos_t node_pos;
    int i;

    if (_node->leaf == 0)
    {
        for (i = 0; i <= _node->value_count; i++)
        {
            aeon_btree_node_save(file, _tree, _node->children[i], update);
            _node->children_positions[i] = _node->children[i]->position;
        }
    }

    // If we are updating a node, seek to its position.
    // If not then we are appending in the current position.
    if (update == 1)
    {
        fseek(file, _node->position, SEEK_SET);
    }
    else
    {
        fgetpos(file, &node_pos);
        _node->position = (unsigned long)node_pos.__pos;
    }

    fwrite(&_node->value_count, sizeof(unsigned int), 1, file);
    fwrite(&_node->leaf, sizeof(unsigned int), 1, file);
    fwrite(_node->keys, sizeof(unsigned long), (2 * _tree->order) - 1, file);
    fwrite(_node->children_positions, sizeof(unsigned long), 2 * _tree->order,
            file);
}

// Saves the entire tree to a new file.
void aeon_btree_save(aeon_btree *_tree, char *_file)
{
    aeon_btree_header header;
    header.magic_byte = TREE_MAGIC_BYTE;
    header.order = _tree->order;
    header.node_size = _tree->node_size;

    FILE *file = fopen(_file, "wb");
    fseek(file, sizeof(aeon_btree_header), SEEK_SET);

    aeon_btree_node_save(file, _tree, _tree->root, 0);
    header.root_position  = _tree->root->position;

    // Write the header.
    fseek(file, 0, SEEK_SET);
    fwrite(&header.magic_byte, sizeof(char), 1, file);
    fwrite(&header.order, sizeof(unsigned int), 1, file);
    fwrite(&header.node_size, sizeof(unsigned int), 1, file);
    fwrite(&header.root_position, sizeof(unsigned long), 1, file);

    fclose(file);
}

void aeon_btree_load(aeon_btree *_tree, char *_file)
{

}

void aeon_btree_split(aeon_btree *_tree, aeon_btree_node *_parent,
        aeon_btree_node *_child, int median_index)
{
    int i;
    aeon_btree_node *new_node = aeon_btree_node_create(_tree);
    new_node->leaf = _child->leaf;
    new_node->value_count = _tree->order - 1;

    // Copy the higher order keys to the new child.
    for (i = 0; i < _tree->order - 1; i++)
    {
        new_node->keys[i] = _child->keys[i + _tree->order];
        if (!_child->leaf)
        {
            new_node->children[i] = _child->children[i + _tree->order];
        }
    }

    // Copy the last child.
    if (_child->leaf == 0)
    {
        new_node->children[i] = _child->children[i + _tree->order];
    }

    _child->value_count = _tree->order - 1;

    for (i = _parent->value_count + 1; i > median_index + 1; i--)
    {
        _parent->children[i] = _parent->children[i - 1];
    }

    _parent->children[median_index + 1] = new_node;

    for (i = _parent->value_count; i > median_index; i--)
    {
        _parent->keys[i] = _parent->keys[i - 1];
    }

    _parent->keys[median_index] = _child->keys[_tree->order - 1];
    _parent->value_count++;
}

void aeon_btree_node_insert(aeon_btree *_tree, aeon_btree_node *_node,
        int value)
{
    int i = _node->value_count - 1;

    if (_node->leaf == 1)
    {
        // Loop down the nodes moving them all up 1 place until the space for the new value is reached.
        while (i >= 0 && value < _node->keys[i])
        {
            _node->keys[i + 1] = _node->keys[i];
            i--;
        }

        // Insert the new value.
        _node->keys[i + 1] = value;
        _node->value_count++;
    }
    else
    {
        // Node is full so need to identify the correct child.

        while (i >= 0 && value < _node->keys[i])
        {
            i--;
        }
        i++;

        if (_node->children[i]->value_count == (2 * _tree->order) - 1)
        {
            aeon_btree_split(_tree, _node, _node->children[i], i);

            if (value > _node->keys[i])
            {
                i++;
            }
        }

        aeon_btree_node_insert(_tree, _node->children[i], value);
    }
}

void aeon_btree_insert(aeon_btree *_tree, int value)
{
    aeon_btree_node *node;
    aeon_btree_node *root = _tree->root;
    if (root->value_count == ((2 * _tree->order) - 1))
    {
        // Node is full, need to split
        node = aeon_btree_node_create(_tree);
        _tree->root = node;
        node->leaf = 0;
        node->children[0] = root;
        aeon_btree_split(_tree, node, root, 0);
        aeon_btree_node_insert(_tree, node, value);
    }
    else
    {
        aeon_btree_node_insert(_tree, root, value);
    }
}
