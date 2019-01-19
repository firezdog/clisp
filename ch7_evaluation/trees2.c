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
    return t;
}

int countBranches(tree* t) {
    int branches = t->num_children;
    for (int i = 0; i < branches; i++) {
        if (t->children[i]) { branches += countBranches(t->children[i]); }
    }
    return branches;
}

void addBranch(tree* t, int data) {
    int children = t->num_children;
    tree **temp = t->children;
    t->children = malloc(100 * sizeof(tree));
    for (int i = 0; i < children; i++) {
        t->children[i] = temp[i];
    }
    t->children[children] = createTree(data);
    t->num_children++;
}

int main() {
    tree* t = createTree(5);
    addBranch(t,10);
    addBranch(t,10);
    addBranch(t,10);
    addBranch(t,10);
    addBranch(t,10);
    addBranch(t->children[0],10);
    addBranch(t->children[0],10);
    addBranch(t->children[0],10);
    addBranch(t->children[0],10);

    addBranch(t->children[1],10);
    addBranch(t->children[1],10);
    // for some reason, adding another branch crashes it?
    // some kind of memory allocation problem -- see above for hack fix.
    // must be because I need to allocate memory not just for number of children, but children of children also!
    // but there's only an issue when I count the number of trees? -- how is that malloc?
    addBranch(t->children[1],10);
    addBranch(t->children[1],10);
    addBranch(t->children[1],10);
    addBranch(t->children[1],10);
    addBranch(t->children[1],10);

    // printf("%i\n", countBranches(t));
    return 0;
}