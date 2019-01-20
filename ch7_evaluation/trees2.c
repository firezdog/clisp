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

void addBranches(tree* t, int data[], int size) {
    for (int i = 0; i < size; i++) {
        addBranch(t, data[i]);
    }
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

// find the maximum number of children spanning one branch
// does this mean find the node with the most children?
int maxSpan(int max, tree* t) {
    if (max < t->num_children) { max = t->num_children; }
    if (!t->num_children) { return max; }
    for (int i = 0; i < t->num_children; i++) {
        max = maxSpan(max, t->children[i]);
    }
    return max;
}

int main() {
    tree* t = createTree(0);
    int arr[] = {1,2,3,4,5,6};
    int arr2[] = {1,2,3,4,5,6,7,8,9,10};
    addBranches(t, arr, sizeof(arr)/sizeof(arr[0]));
    addBranches(t->children[5], arr2, sizeof(arr2)/sizeof(arr2[0]));
    printf("%i\n", maxSpan(0,t));
    return 0;
}