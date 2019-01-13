#include <stdio.h>
#include <stdlib.h>

// start simple with binary tree
typedef struct node {
    int data;
    struct node *left;
    struct node *right;
} node;

// function that returns a pointer
node* create_node(int data) {
    node *new_node = (node*)malloc(sizeof(node));
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

int main() {
    node* root = create_node(1);
    root->left = create_node(2);
    root->left->left = create_node(3);
    printf("%i, %i, %i", root->data, root->left->data, root->left->left->data);
    return 0;
}

// more complicated n-ary tree? (less elegant version)
// struct n_ary {
//     int data;
//     int num_children;
//     struct t **children;
// };