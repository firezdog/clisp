#include <stdio.h>
#include <stdlib.h>

// definition: a leaf is a node with no children
// definition: a branch is a connection between two nodes.
// proposition: the number of branches of a tree is equal to the sum its nodes' children.
// one node cannot be connected to the same child by different branches, and one branch cannot connect a node to different children.

struct tree {
    int data;
    int num_children;
    struct tree** children;
} typedef tree;

tree* createTree(int data) {
    tree* t = malloc(sizeof(tree));
    t->num_children = 0;
    t->data = data;
    t->children = NULL;
    return t;
}

void addBranch(tree* t, int data) {
    int children = t->num_children;
    tree **temp = t->children;
    t->children = malloc((children + 1) * sizeof(tree));
    for (int i = 0; i < children; i++) {
        t->children[i] = temp[i];
    }
    t->children[children] = createTree(data);
    t->num_children++;
}

int countBranches(tree* t) {
    int branches = t->num_children;
    for (int i = 0; i < t->num_children; i++) {
        branches += countBranches(t->children[i]);
    }
    return branches;
}

int countLeaves(tree* t) {
    if (t->num_children == 0) { return 1; }
    int leaves = 0;
    for (int i = 0; i < t->num_children; i++) {
        leaves += countLeaves(t->children[i]);
    }
    return leaves;
}

int main() {
    tree* t = createTree(0);
    addBranch(t, 0);
    addBranch(t, 0);
    addBranch(t, 0);
    addBranch(t->children[0], 0);
    addBranch(t->children[0], 0);
    printf("%i\n", countLeaves(t));
    return 0;
}