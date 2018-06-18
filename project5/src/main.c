#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "tree.h"

char *files_base_path;

void clear_input(){

	char c;
	while ((c = getchar()) != '\n' && c != EOF)
		/* discard */;

}

int get_choice(int min, int max) {
	// get a user choice in range from min to max (inclusive)

	int choice = min-1;

	do {

		printf("\nEscolha uma opcao (%d ate %d): ", min, max);
		scanf("%d", &choice);

		clear_input();		

	} while (choice > max || choice < min);

	return choice;

}

int get_value(){

	int value = 0;
	
	puts("\nDigite um valor: ");
	scanf("%d", &value);
	
	clear_input();

	return value;
}

Tree *getTree(){
	// load tree
	Tree *root = NULL;

	while(root == NULL) {

		printf("\n\nDigite o nome do arquivo: %s", files_base_path);

		char *buffer = (char *) calloc(255, sizeof(char));

		scanf("%s", buffer);

		char *filepath = (char *) calloc(
			strlen(buffer) + strlen(files_base_path) + 16,
			sizeof(char)
		);

		strcat(filepath, files_base_path);
		strcat(filepath, buffer);

		root = loadTreeFromFile(filepath);

		if(root == NULL){
			puts("\nErro na leitura ou arquivo nao encontrado, tente novamente.\n");
		}

		free(buffer);
		free(filepath);
	}

	return root;
}

int main(int argc, char **argv) {
	// argv[0] = (str) program executable
	// argv[1] = (str) dataset path
	// argv[2] = (int) debug mode

	files_base_path = argv[1];

	int base_path_iserted = 0;

	while(files_base_path == NULL){
		puts("\nNo file base path provided!");
		
		puts("\nInsert a new one: ");

		files_base_path = (char *) calloc(255, sizeof(char));
		
		if(files_base_path == NULL){
			puts("Memory error");
			exit(1);
		}

		scanf("%s", files_base_path);

		
		base_path_iserted = 1;
	}

	Tree *tree_root = NULL;

	puts("Iniciar com arvore de arquivo? 1 - sim / 0 - nao");
	int preload = get_choice(0,1);

	if(preload){
		tree_root = getTree();
	}

	// main menu loop
	int run = 1;
	while (run) {

		puts("\n\n==========================\n\n");

		if(tree_root == NULL){
			puts("\n\n\t\tATENCAO: ARVORE NULA/NAO CARREGADA\n\n");
		}

		puts("Operacoes:\n\
		-1 - exit\n\
		0 - loadTreeFromFile\n\
		1 - showTree\n\
		2 - isFull\n\
		3 - searchValue\n\
		4 - getHeight\n\
		5 - removeValue\n\
		6 - printInOrder\n\
		7 - printPreOrder\n\
		8 - printPostOrder\n\
		9 - balanceTree\n\
		10 - insertNode \n\
		11 - nodeCount \n\
		");
		int choice = get_choice(-1, 11);

		if (choice == -1) {
			
			run = 0;

		} else if (choice == 0) {

			if (tree_root != NULL){
				free_tree(tree_root);
			}

			tree_root = getTree();

		}else if (choice == 1) {
			
			showTree(tree_root);

		}else if (choice == 2) {
			
			isFull(tree_root);

		}else if (choice == 3) {

			int value = get_value();

			searchValue(tree_root, value);
		
		}else if (choice == 4) {

			getHeight(tree_root);

		}else if (choice == 5) {

			int value = get_value();

			tree_root = removeValue(tree_root, value);
		}else if (choice == 6) {

			printInOrder(tree_root);

		}else if (choice == 7) {

			printPreOrder(tree_root);

		}else if (choice == 8) {

			printPostOrder(tree_root);

		}else if (choice == 9) {

			//balanceTree(tree_root);
		}else if (choice == 10){

			int value = get_value();

			Tree *node = create_node(value);

			tree_root = insert_node(tree_root, node);

		}else if (choice == 11){

			printf("Numero de elementos: %d", nodeCount(tree_root));
		}
	}

	if(tree_root != NULL) free_tree(tree_root);
	if (base_path_iserted) free(files_base_path);

	return 0;
}
