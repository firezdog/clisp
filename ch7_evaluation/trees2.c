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

void traverse(tree* t) {
    int size = countBranches(t) + 1;
    tree* queue[size];
    for (int i = 0; i < size; i++) {
        queue[i] = NULL;
    }
    queue[0] = t;
    while (queue[0]) {
        tree* current = queue[0];
        printf("%i\n", current->data);
        queue[0] = NULL;
        for (int i = 0; i < size-1; i++) {
            queue[i] = queue[i+1];
        }
        int j = 0;
        while (queue[j]) { j++; }
        for (int i = 0; i < current->num_children; i++) {
            queue[j] = current->children[i];
            j++;
        }
    }
}

int main() {
    tree* t = createTree(0);
    int arr[] = {1, 2, 3, 4, 5};
    int arr2[] = {11, 12, 13, 14, 15};
    int arr3[] = {101, 102, 103, 104, 105};
    addBranches(t, arr, 5);
    addBranches(t->children[4], arr2, 5);
    addBranches(t->children[4]->children[4], arr3, 5);
    traverse(t);
    return 0;
}