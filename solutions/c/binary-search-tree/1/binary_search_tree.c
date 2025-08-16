#include "binary_search_tree.h"
#include <stdlib.h>

static node_t *create_node(int data)
{
    node_t *node = malloc(sizeof(node_t));
    if (!node) {
        return NULL;
    }
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static node_t *insert_node(node_t *root, int data)
{
    if (root == NULL) {
        return create_node(data);
    }
    
    if (data <= root->data) {
        root->left = insert_node(root->left, data);
    } else {
        root->right = insert_node(root->right, data);
    }
    
    return root;
}

static int count_nodes(node_t *tree)
{
    if (tree == NULL) {
        return 0;
    }
    return 1 + count_nodes(tree->left) + count_nodes(tree->right);
}

static void inorder_fill(node_t *tree, int *array, int *index)
{
    if (tree == NULL) {
        return;
    }
    
    inorder_fill(tree->left, array, index);
    array[(*index)++] = tree->data;
    inorder_fill(tree->right, array, index);
}

node_t *build_tree(int *tree_data, size_t tree_data_len)
{
    if (tree_data_len == 0) {
        return NULL;
    }
    
    node_t *root = NULL;
    for (size_t i = 0; i < tree_data_len; i++) {
        root = insert_node(root, tree_data[i]);
    }
    
    return root;
}

void free_tree(node_t *tree)
{
    if (tree == NULL) {
        return;
    }
    
    free_tree(tree->left);
    free_tree(tree->right);
    free(tree);
}

int *sorted_data(node_t *tree)
{
    if (tree == NULL) {
        return NULL;
    }
    
    int node_count = count_nodes(tree);
    int *result = malloc(node_count * sizeof(int));
    if (!result) {
        return NULL;
    }
    
    int index = 0;
    inorder_fill(tree, result, &index);
    
    return result;
}
