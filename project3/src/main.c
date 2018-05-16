#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILENAME "contatos.txt"

// define Contact as a struct Element (we will use it in a list) 
typedef struct Element {

	char *name;
	char *telephone; // format: XXXXX-XXXX
	char *address;
	unsigned int cep;
	char *birthday; // format DD/MM/YYYY

	struct Element *next;
	struct Element *previous;

} Contact;

int debug = 0;

Contact *create_list() {
	// initializes a list

	return NULL;
}

Contact *create_item(char *name, char *telephone, char *address, unsigned int cep, char *birthday) {

	Contact *contact = (Contact *)malloc(sizeof(Contact));

	contact->name = name;
	contact->telephone = telephone;
	contact->address = address;
	contact->cep = cep;
	contact->birthday = birthday;

	contact->next = NULL;
	contact->previous = NULL;

	return contact;
}

Contact *insert_beginning(Contact *list, Contact *value) {

	if (list != NULL) {

		list->previous = value;
		value->next = list;

	}

	return value;
}

void clear_screen() {

	puts("\x1B[2J");
}

void clear_input(){

	char c;
	while ((c = getchar()) != '\n' && c != EOF)
		/* discard */;

}

void print_header() {

	puts("\n\n====================================\nAGENDA\n====================================\n\n");
}

int get_choice(int min, int max) {
	// get a user choice in range from min to max (inclusive)

	int choice = -1;

	do {

		printf("\nEscolha uma opcao (%d ate %d): ", min, max);
		scanf("%d", &choice);

		clear_input();		

	} while (choice > max || choice < min);

	return choice;
}

int valid_name(char *name) {

	int len = strlen(name);

	// max len = 100 char + 1 '\0' = 101
	if (len > 100) {
		return 0;
	}

	// a name cant contain numbers
	for (int i = 0; i < len; i++) {
		if (name[i] > '0' && name[i] < '9') {
			return 0;
		}
	}

	return 1;
}

int valid_address(char *address) {

	int len = strlen(address);

	// max len = 100 char + 1 '\0' = 101
	if (len > 100) {
		return 0;
	}

	return 1;
}


int valid_telephone(char *telephone) {


	int len = strlen(telephone);

	if (len != 10) {
		return 0;
	}

	// the phone must be in the format XXXXX-XXXX
	for (int i = 0; i < len - 1; i++) {
		if (telephone[i] < '0' || telephone[i] > '9') {

			if (i != 5) {
				return 0;
			}
		}

		if (i == 5 && telephone[i] != '-') {
			return 0;
		}
	}

	return 1;

}

int valid_birthday(char *birthday) {


	int len = strlen(birthday);

	if (len != 10) {
		return 0;
	}

	// the birthday must be in the format DD/MM/YYYY
	for (int i = 0; i < len - 1; i++) {
		if (birthday[i] < '0' || birthday[i] > '9') {

			if ((i != 2 && i != 5)) {
				return 0;
			}
		}

		if ((i == 2 || i == 5) && birthday[i] != '/') {
			return 0;
		}
	}

	return 1;

}

void print_contact(Contact *contact) {

	printf("\nNome: %s\nTelefone: %s\nEndereco: %s\nCEP: %d\nAniversario: %s", contact->name, contact->telephone, contact->address, contact->cep, contact->birthday);
}

void print_all_contacts(Contact *list) {

	Contact *current = list;

	if (current == NULL) {
		puts("\nNenhum contato cadastrado\n");
	}

	while (current != NULL) {

		puts("\n---\n");

		print_contact(current);

		current = current->next;

	}

}

char *read_line(FILE *f) {
	// read a line (up to next '\n', will change \n to \0) from a file, it will continue from where the file position is
	// set prior to calling the function

	char *string = (char *)malloc(sizeof(char));

	if (string == NULL) {
		puts("ERROR allocating memory");
		exit(0);
	}

	int i = 0; // line size counter
	char c = fgetc(f);
	while (c != EOF && c != '\n') {

		i++;

		// realloc to be the size needed by the string
		// we user i + 1 to compensate for the \0 at the end
		char *ptr = (char *)realloc(string, i * sizeof(char) + 1);

		if (ptr == NULL) {
			puts("ERROR allocating memory");
			exit(0);
		}

		string = ptr;

		string[i - 1] = c;

		c = fgetc(f);
	}

	string[i] = '\0';

	return string;
}

Contact *get_contacts_from_file() {
	// reads the content of contact file and save in a double linked list 
	// if file does not exist, create it

	FILE *file = fopen(FILENAME, "r");

	if (file == NULL) {

		if (debug) {
			puts("\ncontact file not found\n");
		}

		return NULL;
	}

	if (debug) {
		puts("\nContacts FILE FOUND\n");
	}

	Contact *list = create_list();


	// FILE STRUCTURE:
	// Line 1: name
	// Line 2: telephone
	// Line 3: address
	// Line 4: CEP
	// Line 5: birthday
	// Line 6: $  (divisor)

	char c = fgetc(file);

	if (c != EOF) {
		rewind(file);


		do {
			char *name = read_line(file);

			if (valid_name(name) == 0) {
				puts("\nINVALID NAME in contacts file, exiting");
				puts(name);
				exit(0);
			}

			char *telephone = read_line(file);

			if (valid_telephone(telephone) == 0) {
				puts("\nINVALID TELEPHONE in contacts file, exiting");
				puts(telephone);
				exit(0);
			}

			char *address = read_line(file);

			if (valid_address(address) == 0) {
				puts("\nINVALID ADDRESS in contacts file, exiting");
				puts(address);
				exit(0);
			}

			unsigned int cep;
			fscanf(file, "%d\n", &cep);

			char *birthday = read_line(file);

			if (valid_birthday(birthday) == 0) {
				puts("\nINVALID BIRTHDAY in contacts file, exiting");
				puts(birthday);
				exit(0);
			}

			Contact *contact = create_item(name, telephone, address, cep, birthday);

			if (debug) {
				print_contact(contact);
			}

			list = insert_beginning(list, contact);

			// at the end of a contact there is '$\n' on the file
			// but on the last line, there is no \n

			c = fgetc(file); // get the $ char
			c = fgetc(file); // get the \n char 
		} while (c != EOF);
	}

	fclose(file);

	return list;

}

char *string_tolower(char *string) {

	char *lowercase = (char *)malloc((strlen(string) + 13) * sizeof(char));

	if (lowercase == NULL) {

		puts("\nMEMORY ALLOCATION error");
		exit(0);
	}

	for (int i = 0; i < (int)strlen(string); i++) {
		lowercase[i] = tolower(string[i]);
	}

	return lowercase;

}

int string_compare(char *str1, char *str2){

	char *lower_str1 = string_tolower(str1);
	char *lower_str2 = string_tolower(str2);

	int result = strcmp(lower_str1, lower_str2);

	free(lower_str1);
	free(lower_str2);

	return result;

}

Contact *insertion_sort(Contact *list) {

	if (list == NULL || list->next == NULL) {

		// list have zero or one element, so its ordered

		return list;
	}

	// starting item of sorted list, initialized as NULL because the list is not sorted
	Contact *sorted_root = NULL;

	while (list != NULL) {

		// get the current position of the list and move it fowards
		Contact *current = list;
		list = list->next;

		// if this is the first element or is lower than the first
		if (sorted_root == NULL || string_compare(current->name,sorted_root->name) < 0) {

			// set current as new root
			current->next = sorted_root;
			current->previous = NULL;

			if (sorted_root != NULL) {
				sorted_root->previous = current;
			}

			// set the current as the root of the sorted list
			sorted_root = current;

		}
		else {

			// if the current element is greater than the sorted root

			Contact *test_element = sorted_root;

			while (test_element != NULL) {

				// if test element is the last item in list or the current element is lower than the next
				if (test_element->next == NULL || string_compare(current->name, test_element->next->name) < 0) {

					// insert current element into the list

					// copy next element from test element
					current->next = test_element->next;

					// set current element previous element as test elemente, since we are inserting in front of it
					current->previous = test_element;

					// change next from test element to point to current
					test_element->next = current;

					// if there is a next element after the current one
					if (current->next != NULL) {

						// we want to set the current elemente as the previous elemente of the next element
						current->next->previous = current;
					}

					// inserted
					break;

				}

				test_element = test_element->next;
			}
		}
	}

	return sorted_root;

}

Contact *new_register(Contact *list) {

	char *name = (char *)malloc(101 * sizeof(char));

	if (name == NULL) {

		puts("Memory error");
		exit(0);
	}

	while (1) {

		puts("\nDigite o nome: ");
		scanf("%100[^\n]", name);

		clear_input();

		if (valid_name(name)) {

			break;
		}
		else {

			puts("\n---Nome invalido, tente novamente.---\n\n");
		}
	}

	char *telephone = (char *)malloc(11 * sizeof(char));


	if (telephone == NULL) {

		puts("Memory error");
		exit(0);
	}

	while (1) {

		puts("\nDigite o telefone: ");
		scanf("%s", telephone);

		clear_input();

		if (valid_telephone(telephone)) {

			break;
		}
		else {

			puts("\n---Telefone invalido, tente novamente.---\n\n");
		}

	}

	char *address = (char *)malloc(101 * sizeof(char));


	if (address == NULL) {

		puts("Memory error");
		exit(0);
	}

	while (1) {

		puts("\nDigite o endereco: ");
		scanf("%100[^\n]", address);

		clear_input();

		if (valid_address(address)) {

			break;
		}
		else {

			puts("\n---Endereco invalido, tente novamente.---\n\n");
		}
	}

	unsigned int cep;
	puts("\nDigite o cep: ");
	scanf("%d", &cep);
	clear_input();


	char *birthday = (char *)malloc(11 * sizeof(char));


	if (birthday == NULL) {

		puts("Memory error");
		exit(0);
	}

	while (1) {

		puts("\nDigite o aniversario (DD/MM/AAAA): ");
		scanf("%s", birthday);

		clear_input();


		if (valid_birthday(birthday)) {

			break;
		}
		else {

			puts("\n---Aniversario invalido, tente novamente.---\n\n");
		}
	}

	Contact *contact = create_item(name, telephone, address, cep, birthday);

	list = insert_beginning(list, contact);
	list = insertion_sort(list);

	return list;

}

void free_list(Contact *list) {

	while (list != NULL) {
		Contact *current = list;
		list = list->next;

		free(current->address);
		free(current->birthday);
		free(current->name);
		free(current->telephone);

		free(current);
	}
}

void save_list_to_file(Contact *list) {

	// overwrite file
	FILE *file = fopen(FILENAME, "w");


	if (file == NULL) {

		puts("\nFAILED TO SAVE TO FILE\n");

		exit(0);
	}

	Contact *current = list;
	while (current != NULL) {

		// FILE STRUCTURE:
		// Line 1: name
		// Line 2: telephone
		// Line 3: address
		// Line 4: CEP
		// Line 5: birthday
		// Line 6: $  (divisor)

		fprintf(file, "%s\n", current->name);
		fprintf(file, "%s\n", current->telephone);
		fprintf(file, "%s\n", current->address);
		fprintf(file, "%d\n", current->cep);
		fprintf(file, "%s\n", current->birthday);

		fputc('$', file);

		current = current->next;

		if (current != NULL) fputc('\n', file);

	}

	fclose(file);
}

void print_contact_info(Contact *list) {

	char *search_name = (char *) malloc(101 * sizeof(char));

	puts("\nDigite o nome do contato a ser pesquisado: ");
	scanf("%100[^\n]", search_name);

	printf("\nProcurando por %s...\n", search_name);

	Contact *current = list;

	int found = 0;

	while (current != NULL) {

		if (strstr(current->name, search_name) != NULL) {

			printf("\n...\n\nEncontrado: ''%s'' em:\n", search_name);

			print_contact(current);
		
			found = 1;
		}

		current = current->next;
	}

	if (found == 0)	puts("\nNao encontrado\n");

}

Contact *remove_contact(Contact *list) {

	char* search_name = (char *) malloc(101 * sizeof(char));

	puts("\nDigite o nome do contato a ser removido: ");
	scanf("%100[^\n]", search_name);

	clear_input();

	printf("\nProcurando por %s...\n", search_name);

	Contact *current = list;
	Contact *iterator = list;

	Contact *return_list = NULL;

	int found = 0;

	while (iterator != NULL) {

		current = iterator;
		iterator = iterator->next;

		if (strstr(current->name, search_name) != NULL) {

			printf("\n...\n\nEncontrado ''%s'' em:\n", search_name);

			print_contact(current);

			// if current is not the list root
			if (current->previous != NULL) {

				// remove connection from previous and point it to
				// current next 
				current->previous->next = current->next;

				// if current have a next, set the next previous as current 
				// previous  
				if (current->next != NULL) {
					current->next->previous = current->previous;
				}

				// since we havent changed the root, we set the list origin to
				// be returned
				return_list = list;

			} else {

				// if we are the root

				// set the new root as the next item
				Contact *new_root = current->next;

				// remove connection to current object
				if(new_root != NULL) { // only if list is still populated
					new_root->previous = NULL;
				}
				// prepare the new list (sorted) without the current element
				return_list = insertion_sort(new_root);
			}

			// clean the removed element connections
			current->next = NULL;
			current->previous = NULL;

			// free memory
			free_list(current);


			puts("\n---\nContato removido. Nao esqueca de salvar as alteracoes.\n---\n");


			found = 1;
		}
	}

	if(found == 0) {
		puts("\nNao encontrado\n");
	}

	return found ? return_list : list;
}

int main(int argc, char **argv) {
	// argv[0] = (str) program executable
	// argv[1] = (int) debug mode


	if (argc > 1) {

		// convert debug var from char to int
		char *c = argv[1];
		debug = (int)strtol(c, NULL, 10);

		if (debug) {
			printf("Debug: %d\n", debug);
		}
	}

	// get contacts from file
	Contact *contact_list = get_contacts_from_file();

	// ordenate using insertion sort
	contact_list = insertion_sort(contact_list);


	// main menu loop
	int run = 1;
	while (run) {

		clear_screen();

		print_header();

		puts("Operacoes:\n1 - Novo registro\n2 - Remover registro\n3 - Vizualizar registro\n4 - Vizualizar todos os registros\n5 - Salvar e sair\n");
		int choice = get_choice(1, 5);

		puts("\n\n==========================\n\n");

		if (choice == 1) {

			contact_list = new_register(contact_list);

		}
		else if (choice == 2) {
			contact_list = remove_contact(contact_list);
		}
		else if (choice == 3) {
			print_contact_info(contact_list);
		
			clear_input();
		}
		else if (choice == 4) {

			print_all_contacts(contact_list);

		}
		else if (choice == 5) {

			run = 0;
			save_list_to_file(contact_list);
			free_list(contact_list);

		}

		puts("\n\n---\nAperte enter para continuar...");
		getchar();

	}

	return 0;
}
