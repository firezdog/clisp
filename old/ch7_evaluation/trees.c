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

// more complicated n-ary tree? (less elegant version)
typedef struct n_ary {
    int data;
    int num_children;
    struct n_ary** children;
} n_ary;

n_ary* create_n_ary(int data, int num_children) {
    n_ary* new_n_ary;
    new_n_ary = malloc(sizeof(n_ary));
    new_n_ary->num_children = num_children;
    // important: remember to allocate memory!
    new_n_ary->children = malloc(num_children * sizeof(n_ary));
    for (int i = 0; i < num_children; i++) {
        new_n_ary->children[i] = create_n_ary(0, 0);
    }
    return new_n_ary;
}

void create_some_binary_nodes() {
    node* root = create_node(1);
    root->left = create_node(2);
    root->left->left = create_node(3);
    printf("%i, %i, %i", root->data, root->left->data, root->left->left->data);
}

int main() {
    printf("Making head for n-ary tree.\n");
    n_ary* t = create_n_ary(5, 10);
    printf("The head has %i children.\n", t->num_children);
    for (int i = 0; i < t->num_children; i++) {
        printf("Child %i has %i children.\n", i+1, t->children[i]->num_children);
    }
}