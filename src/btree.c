#include <stdlib.h>
#include "btree.h"

aeon_btree_node *aeon_btree_node_create(aeon_btree *tree)
{
    aeon_btree_node *node;

    node = malloc(sizeof(aeon_btree_node));

    node->values = malloc(sizeof(int) * ((2 * tree->order) - 1));
    node->value_count = 0;
    node->leaf = 1;
    node->children = calloc(2 * tree->order, sizeof(aeon_btree_node));
    node->parent = NULL;

    return node;
}

aeon_btree *aeon_btree_create(int order)
{
    aeon_btree *tree;

    tree = malloc(sizeof(aeon_btree));

    tree->order = order;
    tree->root = aeon_btree_node_create(tree);

    return tree;
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
        new_node->values[i] = _child->values[i + _tree->order];
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
        _parent->children[i] = _parent->children[i-1];
    }

    _parent->children[median_index + 1] = new_node;

    for (i = _parent->value_count; i > median_index; i--)
    {
        _parent->values[i] = _parent->values[i-1];
    }

    _parent->values[median_index] = _child->values[_tree->order - 1];
    _parent->value_count++;
}

void aeon_btree_node_insert(aeon_btree *_tree, aeon_btree_node *_node,
        int value)
{
    int i = _node->value_count - 1;

    if (_node->leaf == 1)
    {
        // Loop down the nodes moving them all up 1 place until the space for the new value is reached.
        while (i >= 0 && value < _node->values[i])
        {
            _node->values[i + 1] = _node->values[i];
            i--;
        }

        // Insert the new value.
        _node->values[i + 1] = value;
        _node->value_count++;
    }
    else
    {
        // Node is full so need to identify the correct child.

        while (i >= 0 && value < _node->values[i])
        {
            i--;
        }
        i++;

        if (_node->children[i]->value_count == (2 * _tree->order) - 1)
        {
            aeon_btree_split(_tree, _node, _node->children[i], i);

            if (value > _node->values[i])
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
