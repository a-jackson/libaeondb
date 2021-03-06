#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "btree.h"

// Creates a single node
aeon_btree_node *aeon_btree_node_create(aeon_btree *tree)
{
    aeon_btree_node *node;

    node = malloc(sizeof(aeon_btree_node));

    node->keys = malloc(sizeof(KEY_TYPE) * ((2 * tree->order) - 1));
    node->values = malloc(sizeof(KEY_TYPE) * ((2 * tree->order) - 1));
    node->value_count = 0;
    node->leaf = 1;
    node->children = calloc(2 * tree->order, sizeof(aeon_btree_node));
    node->children_positions = malloc(sizeof(aeon_pos_t) * 2 * tree->order);
    node->position = 0;

    return node;
}

// Create a tree
aeon_btree *aeon_btree_create(uint32_t order, char *_file, int file_length)
{
    aeon_btree *tree;
    aeon_size_t node_size;
    char *file;

    file = malloc(sizeof(char) * file_length);
    memcpy(file, _file, sizeof(char) * file_length);

    tree = malloc(sizeof(aeon_btree));

    tree->order = order;
    tree->root = aeon_btree_node_create(tree);

    node_size = sizeof(KEY_TYPE) * ((2 * order) - 1); // keys
    node_size += sizeof(VALUE_TYPE) * ((2 * order) - 1); // Values
    node_size += sizeof(aeon_count_t) + sizeof(uint32_t); // value_count & leaf
    node_size += sizeof(aeon_pos_t) * 2 * order; // children_positions
    tree->node_size = node_size;

    tree->file_name = file;

    return tree;
}

// Saves a node to file
void aeon_btree_node_save(aeon_btree *_tree, aeon_btree_node *_node)
{
    fpos_t node_pos;

    // If there's no position it must new so add at current location
    if (_node->position != 0)
    {
        fseek(_tree->file, _node->position, SEEK_SET);
    }
    else
    {
        fgetpos(_tree->file, &node_pos);
        _node->position = (aeon_pos_t) node_pos.__pos;
    }

    fwrite(&_node->value_count, sizeof(aeon_count_t), 1, _tree->file);
    fwrite(&_node->leaf, sizeof(uint32_t), 1, _tree->file);
    fwrite(_node->keys, sizeof(KEY_TYPE), (2 * _tree->order) - 1, _tree->file);
    fwrite(_node->values, sizeof(VALUE_TYPE), (2 * _tree->order) - 1,
            _tree->file);
    fwrite(_node->children_positions, sizeof(aeon_pos_t), 2 * _tree->order,
            _tree->file);
    fflush(_tree->file);
}

// Saves the entire tree to a new file.
void aeon_btree_save(aeon_btree *_tree, int header_only)
{
    aeon_btree_header header;
    header.order = _tree->order;
    header.node_size = _tree->node_size;

    fseek(_tree->file, sizeof(aeon_btree_header), SEEK_SET);

    if (header_only == 0)
    {
        aeon_btree_node_save(_tree, _tree->root);
    }

    header.root_position = _tree->root->position;

    // Write the header.
    fseek(_tree->file, 0, SEEK_SET);
    fwrite(&header.order, sizeof(uint32_t), 1, _tree->file);
    fwrite(&header.node_size, sizeof(aeon_size_t), 1, _tree->file);
    fwrite(&header.root_position, sizeof(aeon_pos_t), 1, _tree->file);
    fflush(_tree->file);
}

// Loads a single node from the current file position.
void aeon_btree_node_load(aeon_btree *_tree, aeon_btree_node *_node)
{
    fseek(_tree->file, _node->position, SEEK_SET);
    fread(&_node->value_count, sizeof(aeon_count_t), 1, _tree->file);
    fread(&_node->leaf, sizeof(uint32_t), 1, _tree->file);
    fread(_node->keys, sizeof(KEY_TYPE), (2 * _tree->order) - 1, _tree->file);
    fread(_node->values, sizeof(VALUE_TYPE), (2 * _tree->order) - 1,
            _tree->file);
    fread(_node->children_positions, sizeof(aeon_pos_t), 2 * _tree->order,
            _tree->file);
}

// Loads a tree and the root node from the file.
aeon_btree *aeon_btree_load(char *_file, int file_length)
{
    FILE *file = fopen(_file, "rb");
    aeon_btree_header header;
    aeon_btree *tree;

    fread(&header.order, sizeof(uint32_t), 1, file);
    fread(&header.node_size, sizeof(aeon_size_t), 1, file);
    fread(&header.root_position, sizeof(aeon_pos_t), 1, file);

    tree = aeon_btree_create(header.order, _file, file_length);
    tree->file = file;

    tree->root->position = header.root_position;
    aeon_btree_node_load(tree, tree->root);
    fclose(file);
    tree->file = NULL;

    return tree;
}

// Splits the tree when a node is full.
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
        new_node->values[i] = _child->values[i + _tree->order];
        if (_child->leaf == 0)
        {
            new_node->children[i] = _child->children[i + _tree->order];
            new_node->children_positions[i] = _child->children_positions[i
                    + _tree->order];
        }
    }

    // Copy the last child.
    if (_child->leaf == 0)
    {
        new_node->children[i] = _child->children[i + _tree->order];
        new_node->children_positions[i] = _child->children_positions[i
                + _tree->order];
    }

    _child->value_count = _tree->order - 1;

    aeon_btree_node_save(_tree, _child);

    for (i = _parent->value_count + 1; i > median_index + 1; i--)
    {
        _parent->children[i] = _parent->children[i - 1];
        _parent->children_positions[i] = _parent->children_positions[i - 1];
    }

    aeon_btree_save_new_node(_tree, new_node);

    _parent->children[median_index + 1] = new_node;
    _parent->children_positions[median_index + 1] = new_node->position;

    for (i = _parent->value_count; i > median_index; i--)
    {
        _parent->keys[i] = _parent->keys[i - 1];
    }

    _parent->keys[median_index] = _child->keys[_tree->order - 1];
    _parent->values[median_index] = _child->values[_tree->order - 1];
    _parent->value_count++;
    aeon_btree_node_save(_tree, _parent);
    aeon_btree_node_free(new_node);
    _parent->children[median_index + 1] = NULL;
}

// Inserts a key to the tree.
void aeon_btree_node_insert(aeon_btree *_tree, aeon_btree_node *_node,
        KEY_TYPE key, VALUE_TYPE value)
{
    int i = _node->value_count - 1;

    if (_node->leaf == 1)
    {
        // Loop down the nodes moving them all up 1 place until the space for the new value is reached.
        while (i >= 0 && key < _node->keys[i])
        {
            _node->keys[i + 1] = _node->keys[i];
            _node->values[i + 1] = _node->values[i];
            i--;
        }

        // Insert the new value.
        _node->keys[i + 1] = key;
        _node->values[i + 1] = value;
        _node->value_count++;
        aeon_btree_node_save(_tree, _node);
    }
    else
    {
        // Node is full so need to identify the correct child.

        while (i >= 0 && key < _node->keys[i])
        {
            i--;
        }
        i++;

        // Load the child at i
        _node->children[i] = aeon_btree_node_create(_tree);
        _node->children[i]->position = _node->children_positions[i];
        aeon_btree_node_load(_tree, _node->children[i]);

        if (_node->children[i]->value_count == (2 * _tree->order) - 1)
        {
            aeon_btree_split(_tree, _node, _node->children[i], i);

            if (key > _node->keys[i])
            {
                // Need the next child. Free this one.
                aeon_btree_node_free(_node->children[i]);
                _node->children[i] = NULL;
                i++;
                //Load next child.
                _node->children[i] = aeon_btree_node_create(_tree);
                _node->children[i]->position = _node->children_positions[i];
                aeon_btree_node_load(_tree, _node->children[i]);
            }
        }

        aeon_btree_node_insert(_tree, _node->children[i], key, value);

        aeon_btree_node_free(_node->children[i]);
        _node->children[i] = NULL;
    }
}

void aeon_btree_save_new_node(aeon_btree *_tree, aeon_btree_node *_node)
{
    fseek(_tree->file, 0L, SEEK_END);
    aeon_btree_node_save(_tree, _node);
}

// Inserts a value to the tree.
void aeon_btree_insert(aeon_btree *_tree, KEY_TYPE key, VALUE_TYPE value)
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
        node->children_positions[0] = root->position;

        // Save the new node to file.
        aeon_btree_save_new_node(_tree, node);

        // Update the header for new root.
        aeon_btree_save(_tree, 1);

        aeon_btree_split(_tree, node, root, 0);
        aeon_btree_node_insert(_tree, node, key, value);

        // Only need to leave root in memory so can unload the old root
        aeon_btree_node_free(root);
        node->children[0] = NULL;
    }
    else
    {
        aeon_btree_node_insert(_tree, root, key, value);
    }
}

int aeon_btree_node_get_value(aeon_btree *_tree, aeon_btree_node *_node,
        KEY_TYPE key, VALUE_TYPE *value)
{
    int result;
    int i = 0;
    while (i < _node->value_count && key > _node->keys[i])
    {
        i++;
    }

    if (i < _node->value_count && key == _node->keys[i])
    {
        *value = _node->values[i];
        result = 1;
    }
    else if (_node->leaf == 1)
    {
        result = 0;
    }
    else
    {
        _node->children[i] = aeon_btree_node_create(_tree);
        _node->children[i]->position = _node->children_positions[i];
        aeon_btree_node_load(_tree, _node->children[i]);
        result = aeon_btree_node_get_value(_tree, _node->children[i], key,
                value);
        aeon_btree_node_free(_node->children[i]);
        _node->children[i] = NULL;
    }

    return result;
}

int aeon_btree_get_value(aeon_btree *_tree, KEY_TYPE key, VALUE_TYPE *value)
{
    return aeon_btree_node_get_value(_tree, _tree->root, key, value);
}

void aeon_btree_open(aeon_btree *_tree, int new_file)
{
    if (new_file == 1)
    {
        _tree->file = fopen(_tree->file_name, "wb");
        fclose(_tree->file);
    }

    _tree->file = fopen(_tree->file_name, "r+b");
}

void aeon_btree_close(aeon_btree *_tree)
{
    if (_tree->file == NULL) {
        return;
    }

    fclose(_tree->file);
    _tree->file = NULL;
}

// Frees a tree.
void aeon_btree_free(aeon_btree *_tree)
{
    aeon_btree_close(_tree);
    aeon_btree_node_free(_tree->root);
    free(_tree);
}

// Frees a tree node.
void aeon_btree_node_free(aeon_btree_node *_node)
{
    if (_node == NULL )
    {
        return;
    }

    int i;
    free(_node->keys);
    free(_node->values);
    free(_node->children_positions);

    if (_node->leaf == 0)
    {
        for (i = 0; i <= _node->value_count; i++)
        {
            aeon_btree_node_free(_node->children[i]);
            _node->children[i] = NULL;
        }
    }

    free(_node->children);

    free(_node);
}

aeon_btree *aeon_btree_initialise(uint32_t order, char *_file, int file_length)
{
    aeon_btree *tree;
    FILE *file;
    file = fopen(_file, "rb");
    if (file == 0) {
        tree = aeon_btree_create(order, _file, file_length);
        aeon_btree_open(tree, 1);
        aeon_btree_save(tree, 0);
        aeon_btree_close(tree);
        aeon_btree_free(tree);
    }

    return aeon_btree_load(_file, file_length);
}
