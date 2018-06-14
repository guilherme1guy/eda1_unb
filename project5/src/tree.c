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

			printf("Valor e a raiz, não ha pai ou irmaos.");
			break;
		} else if (value < current->value) {

			if (current->left != NULL) {
				
				if (current->left->value == value) {
					printf(
						"\nEncontrado valor %d. Pai: %d. %s",
						current->left->value,
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
					printf(
						"\nEncontrado valor %d. Pai: %d. %s",
						current->right->value,
						current->value
					);

					(current->left != NULL) ? printf("Irmao: %d.", current->left->value) : puts("Nao ha irmao.\n");
				}
			}

			current = current->right;
		}
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