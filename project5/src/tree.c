#include "tree.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PREORDER 0
#define INORDER 1
#define POSTORDER 2

#define SEPARATOR_CHAR ' '

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

    for(int i = 0; i <= data_count; i++){
    
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

Tree *searchElement(Tree *root, int value) {

	Tree *current = root;

	while (1) {

		if (current == NULL) {
			return NULL;
		}

		if (current->value == value) {

			return current;
		} else if (value < current->value) {

			current = current->left;
		}
		else {

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

void getHeight(Tree *root){

    printf("\nAltura: %d", __getHeight(root));
}

int __getHeight(Tree *root){

    int tree_height = 0;

    if (isLeaf(root)){
        return 1;
    }

	if (root == NULL) {
		return 0;
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

	if (node == NULL) return 0;

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

int nodeCount(Tree *root){

    if (isLeaf(root)) return 1;

	if (root == NULL) return 0;

    return 1 + nodeCount(root->left) + nodeCount(root->right);

}

int findLevel(Tree *root, int value){

	int level = 0;

	Tree *current = root;

	while (1) {

		if (current == NULL) {
			return -1;
		}

		if (current->value == value) {

			return level;
		}
		else if (value < current->value) {

			current = current->left;
		}
		else {

			current = current->right;
		}

		level++;
	}
}

int getNumLength(int num){
    int n = 0;
    
    while(num) {
        num /= 10;
        n++;
    }

    return n;
}

void __printMap(char **map, int *relation_map_value, Tree *root){

    int k = 0;
    
    for(int i = 0; map[i] != NULL; i++){
        
        printf("\n");

		int space_count = 0;

        int spaces_to_skip = 0;
        for(int j = 0; j < strlen(map[i]); j++) {
            
            char c = map[i][j];

            if(c == '*'){
                
                int num = -999;

				for (int m = 0; m < 3 * nodeCount(root); m += 3) {
				
					if (relation_map_value[m] == i && relation_map_value[m + 1] == j) {
						num = relation_map_value[m + 2];
					}

				}

                spaces_to_skip = getNumLength(num) - 1;
                


                printf("%d", num);
                k++;
            }else{

                if(c == SEPARATOR_CHAR && spaces_to_skip > 0){
                    spaces_to_skip--;
                }else{
                    printf("%c", c);
                }
            }
            if (c == SEPARATOR_CHAR) space_count++;

        }

        if(space_count == strlen(map[i])) break;
        
    }
    
}

int sum_to_num(int n){

    if(n == 0){
        return 0;
    }

    return n + sum_to_num(n - 1);

}

int __find_magic_num(Tree *root){

    if(isLeaf(root)) return 1;
    
    int num = __getHeight(root) * childCount(root) + 1;

    if(root->left != NULL) num += __find_magic_num(root->left);
    if(root->right != NULL) num += __find_magic_num(root->right);

    return num;
}


void __drawOnMap(int* relational_map_value, int *inserted, char **map, Tree *real_root, Tree *root, int lin, int col){



    map[lin][col] = '*';

    relational_map_value[inserted[0]] = lin;
    relational_map_value[inserted[0] + 1] = col;
    relational_map_value[inserted[0] + 2] = root->value;
    inserted[0] += 3;
    
    if(isLeaf(root)) return;

    int tree_height = __find_magic_num(root) / 4;

    if(root->left != NULL){

        for(int i = 1; i <= tree_height; i++){
            map[lin + i][col - i] = '/';
        }

        __drawOnMap(relational_map_value, inserted, map, real_root, root->left, lin + tree_height + 1, col - tree_height - 1);

    }
    
    if(root->right != NULL){

        for(int i = 1; i <= tree_height; i++){
            map[lin + i][col + i] = '\\';
        }

        __drawOnMap(relational_map_value, inserted, map, real_root, root->right, lin + tree_height + 1, col + tree_height + 1);

    }

}

char **__resize_map_limits(char **map, int *relation_map_value, int size, int lin, int col){

    int min_lin = 0, max_lin = 0, min_col = col, max_col = 0;

    for(int i = 0; i < lin - 1; i++){

		int space_count = 0;

        for(int j = 0; j < col - 1; j++) {
            
            char c = map[i][j];

            if (c == SEPARATOR_CHAR)
                space_count++;
            else{
				
				if (j < min_col) {
					min_col = j;
				}
			
				if (j > max_col) {
					max_col = j;
				}	
			}

        }

        if(space_count == col - 1){
            max_lin = i;
            break;
        }
        
    }

    int new_lin = abs(max_lin - min_lin) + 1;

    int new_col = abs(max_col + 1 - min_col) + 1;

    char **new_map = (char **) calloc(new_lin, sizeof(char *));

	if (new_map == NULL) {
		puts("\nMemory error - new_map");
		exit(1);
	}

    for(int i = 0; i < new_lin - 1; i++){
        new_map[i] = (char *) calloc(new_col, sizeof(char));

		if (new_map[i] == NULL) {
			puts("\nMemory error - new_map");
			exit(1);
		}

		for(int j = 0; j < new_col - 1; j++){
			new_map[i][j] = map[min_lin + i][min_col + j];
		}
	}

	for (int i = 0; i < size; i += 3) {
		relation_map_value[i] -= min_lin;
		relation_map_value[i + 1] -= min_col;
	}


    return new_map;
}

void showTree(Tree *root){

    int tree_height = __getHeight(root); 

    int magic_num = __find_magic_num(root);

    int map_col = 1 + (tree_height + sum_to_num(tree_height) * magic_num) * 2;
    int map_lin = 1 + ceill(
        sqrt( pow(map_col, 2) + pow(map_col/2, 2) )
    );

    char **map = (char **) calloc(map_lin, sizeof(char *));

	if (map == NULL) {
		puts("\nMemory error - MAP");
		exit(1);
	}

    for(int i = 0; i < map_lin - 1; i++){
        map[i] = (char *) calloc(map_col, sizeof(char));

		if (map[i] == NULL) {
			puts("\nMemory error - MAP");
			exit(1);
		}

		for(int j = 0; j < map_col - 1; j++){
			map[i][j] = SEPARATOR_CHAR;
		}
	}

    //i,j,val
    int *relation_map_value = (int *) calloc(3*nodeCount(root), sizeof(int));
    int inserted = 0;

    __drawOnMap(relation_map_value, &inserted, map, root, root, 0, (map_lin/2) - 1);

    char **map_resized = __resize_map_limits(map, relation_map_value, 3*nodeCount(root), map_lin, map_col);

    for(int i = 0; map[i] != NULL; i++){
        free(map[i]);
    }

    free(map);

    __printMap(map_resized, relation_map_value, root);

    
    for(int i = 0; map_resized[i] != NULL; i++){
        free(map_resized[i]);
    }

    free(map_resized);
    free(relation_map_value);

}


Tree *findFather(Tree *root, int value){

	Tree *current = root;

	while (1) {

		if (current == NULL || current->value == value) {
			return NULL;
		}
        
        
         if (value < current->value) {

			if (current->left != NULL) {
				
				if (current->left->value == value) {
                    return current;
                }
			}

			current = current->left;
		}
		else {

			if (current->right != NULL) {

				if (current->right->value == value) {
		            return current;
				}
			}

			current = current->right;
		}
	}
}

Tree *findSuccessor(Tree *root){

    Tree *current = root;
    int go_right = 0;
    
    if(root->right != NULL){
        current = root->right;
    }else if(root->left != NULL){
        current = root->left;
        go_right = 1;
    }else{
        return NULL;
    }

    while(1){

        if(
			isLeaf(current) ||
			(go_right && current->right == NULL) ||
			(!go_right && current->left== NULL)
		) return current;

        if(go_right) current = current->right; else current = current->left;

    }


}

Tree *removeValue(Tree *root, int value){

    Tree *element = searchElement(root, value);
    Tree *father = findFather(root, value);


    int is_root = (element == root);
    
    if(element == NULL){

        printf("\nElemento %d nao encontrado na arvore.", value);

    }else if(isLeaf(element)){
        
        if(father != NULL) {

            if(father->right == element) father->right = NULL;
            if(father->left == element) father->left = NULL;

        }

        free_node(element);

        if(is_root) return NULL;

    } else if(childCount(element) == 1){

        Tree *child = element->right != NULL ? element->right : element->left;

        if(father != NULL){

            if(father->right == element) father->right = child;
            if(father->left == element) father->left = child;

        }else{

            root = child;
        }

        free_node(element);

    }else if(childCount(element) == 2){

        Tree *next = findSuccessor(element);
        int suc_value = next->value;

        removeValue(root, next->value);
    
		element->value = suc_value;
	}


    return root;

}

int balanced(Tree *root){
    
    if(isLeaf(root) || root == NULL){
        return 1;
    }

    int hleft = __getHeight(root->left);
    int hright = __getHeight(root->right);

    if (abs(hright - hleft) > 1){
        return 0;
    }

    return balanced(root->right) && balanced(root->left);
    

}

void rotateRight(Tree *grampa, Tree *father, Tree *son){

    if(grampa != NULL){

        if (grampa->left == father) {
            grampa->left = son;
        } else { 
            grampa->right = son;
        }

    }

    father->left = son->right;
    son->right = father;

}

void rotateLeft(Tree *grampa, Tree *father, Tree *son){

    if(grampa != NULL){

        if (grampa->left == father) {
            grampa->left = son;
        } else { 
            grampa->right = son;
        }

    }

    father->right = son->left;
    son->left = father;

}


Tree *balanceTree(Tree *root, int showSteps){

    if(balanced(root)) return root;



	Tree *work_root = root;
    Tree *gramp = NULL;
    Tree *father = NULL;
    Tree *son = work_root;

    while(son != NULL){

        if (showSteps) showTree(work_root);

        if(son->left){
            

			father = son;
			son = son->left;
			rotateRight(gramp, father, son);

			if (father == work_root) {
				work_root = son;
				gramp = NULL;
				father = NULL;
			}

        }else{
            
            gramp = son;
            son = son->right;
        }

    }


    gramp = NULL;
    father = NULL;
    son = work_root;

    while(son != NULL){

		if (showSteps) showTree(work_root);

        if(gramp != NULL) gramp = father;
        father = son;
        son = son->right;

		if (son == NULL) break;

        if(father == work_root){
			work_root = son;
        }
        
        rotateLeft(gramp, father, son);
        gramp = father;
        father = son;
        son = son->right;

    }


	gramp = NULL;
	father = NULL;
	son = work_root;

	while (son != NULL) {

		if (showSteps) showTree(work_root);

		if (gramp != NULL) gramp = father;
		father = son;
		son = son->right;

		if (son == NULL) break;

		if (father == work_root) {
			work_root = son;
		}

		rotateLeft(gramp, father, son);
		gramp = father;
		father = son;
		son = son->right;

	}


    return work_root;

}