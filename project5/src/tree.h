#ifndef TRRE_H
#define TRRE_H

extern const int initial_time[2];
extern const int timestep_size;

typedef struct tree {
    int value;

    struct tree *left;
    struct tree *right;
} Tree;

Tree *create_node(int value);

void free_node(Tree *node);

void free_tree(Tree *root);

Tree *insert_node(Tree *root, Tree *node);

Tree *removeValue(Tree *root, int value);

Tree *loadTreeFromFile(char *filepath);

void showTree(Tree *root);

void isFull(Tree *root);

void searchValue(Tree *root, int value);

void getHeight(Tree *root);

void printInOrder(Tree *root);

void printPreOrder(Tree *root);

void printPostOrder(Tree *root);

Tree balanceTree(Tree *root);

#endif /* TRRE_H */