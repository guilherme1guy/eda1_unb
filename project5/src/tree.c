#include "tree.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PREORDER 0
#define INORDER 1
#define POSTORDER 2

Tree *create_node(int value){

    Tree *node = (Tree *) calloc(1, sizeof(Tree));

    if(node == NULL){
        puts("\n Memory alocation error - TREE NODE");
        exit(1);
    }

    node->value = value;

    return node;

}

void free_node(Tree *node){

    node->left = NULL;
    node ->right = NULL;

    free(node);

}

Tree *insert_node(Tree *root, Tree *node){

    if (root == NULL) return node;

    if(node->value == root->value){
        return root;
    }
    
    Tree *current = root;
    
    while(1){

        if (node->value > current->value){
            
            if(current->right == NULL){
				current->right = node;
                break;
            }
            
            current = current->right;
        }else if(node->value < current->value){
            
            if(current->left == NULL){
				current->left = node;
                break;
            }
            
            current = current->left;
        }else{

            break;
        }
    }

    return root;
}

Tree *loadTreeFromFile(char *filepath){

    Tree *root = NULL;

    FILE *f = fopen(filepath, "r");
    
    if(f == NULL){
        return NULL;
    }

    int data_count = 0;

    char c = fgetc(f);
    while (c != EOF){
        
        if (c == ' '){
            data_count++;
        }

        c = fgetc(f);
    }

    rewind(f);

    for(int i = 0; i < data_count; i++){
    
        int value;
        fscanf(f, "%d", &value);

        Tree *node = create_node(value);

        root = insert_node(root, node);
    }


    fclose(f);

    return root;
}

void __printTree(Tree *root, int order){

    if (order == PREORDER) printf(" %d, ", root->value);

    if(root->left != NULL){
        __printTree(root->left, order);
    }

    if (order == INORDER) printf(" %d, ", root->value);

    if(root->right != NULL){
        __printTree(root->right, order);
    }
    
    if (order == POSTORDER) printf(" %d, ", root->value);

}

void searchValue(Tree *root, int value) {

	int level = 0;

	Tree *current = root;

	while (1) {

		if (current == NULL) {
			printf("\nNao encontrado.");
			break;
		}

		if (current->value == value) {

			printf("Valor e a raiz, nao ha pai ou irmaos. Nivel: %d", level);
			break;
		} else if (value < current->value) {

			if (current->left != NULL) {
				
				if (current->left->value == value) {
					level++;
					printf(
						"\nEncontrado valor %d, nivel: %d. Pai: %d. ",
						current->left->value,
						level,
						current->value
					);

					(current->right != NULL) ? printf("Irmao: %d.", current->right->value) : puts("Nao ha irmao.\n");
					
					break;
				}
			}

			current = current->left;
		}
		else {

			if (current->right != NULL) {

				if (current->right->value == value) {
					level++;
					printf(
						"\nEncontrado valor %d, nivel: %d. Pai: %d. ",
						current->right->value,
						level,
						current->value
					);

					(current->left != NULL) ? printf("Irmao: %d.", current->left->value) : puts("Nao ha irmao.\n");
				}
			}

			current = current->right;
		}

		level++;
	}

}

void printInOrder(Tree *root){

    puts("\n[ ");

    __printTree(root, INORDER);

    puts("]\n");


}

void printPreOrder(Tree *root){

    puts("\n[ ");

    __printTree(root, PREORDER);

    puts("]\n");


}

void printPostOrder(Tree *root){
    
    puts("\n[ ");

    __printTree(root, POSTORDER);

    puts("]\n");

}

void free_tree(Tree *root){

    
    if(root->left != NULL){
        free_tree(root->left);
    }

    if(root->right != NULL){
        free_tree(root->right);
    }

    free_node(root);

}

void getHeight(Tree *root){

    printf("\nAltura: %d", __getHeight(root));
}

int __getHeight(Tree *root){

    int tree_height = 0;

    if (isLeaf(root)){
        return 1;
    }

    if(root->left != NULL){
        tree_height = __getHeight(root->left);
    }

    if(root->right != NULL){
        int h = __getHeight(root->right);
        
        if(h > tree_height) tree_height = h;
    }


    return tree_height + 1;
}

int isLeaf(Tree *node){
    return (node != NULL &&node->left == NULL && node->right == NULL);
}

int childCount(Tree *node){
    int child = 0;

    if(node->left != NULL) child++;
    if(node->right != NULL) child++;

    return child;
}

void isFull(Tree *root){

    printf("\nA arvore %s", __isFull(root) ? "e cheia" : "NAO e cheia");

}

int __isFull(Tree *root){

    if(root == NULL){
        return 0;
    }

    if(isLeaf(root)){
        return 1;
    }

    return __isFull(root->left) && __isFull(root->right);
}