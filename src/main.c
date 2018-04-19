#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug = 0;
char *dataset_path;

int img_lin = 0;
int img_col = 0;

void exit_with_error(char* error, int code){
    printf("%s\n", error);
    exit(code);
}

void free_data(int** ptr, int len){

    if(debug)
        puts("Freeing members of **ptr");
    for(int i = 0; i < len; i++){
        free(ptr[i]);
    }

    if(debug)
        puts("Freeing **ptr");
    
    free(ptr);
}

int* read_txt_file(char* filename){
// open a file from dataset indicaed by filename, read its contents
// and returns a matrix with it
    

    FILE* file = fopen(filename, "r");

    if(file == NULL){
        char error[255] = "Cannot open file: ";
        strcat(error, filename);
        exit_with_error(error, 1);
    }

    // all the files have the same length of data, so we only
    // need to find the size for the first image/txt

    if(img_lin == 0){
        // find the size of images

        char ch = 0;
        
        int find_col = 1;
        
        while (feof(file) == 0){
 
            ch = fgetc(file);

            // we will user the ; char to find the aumont of
            // data per line
            if(find_col && ch == ';')
                img_col++;
            
            if (ch == '\n'){
               img_lin++;
               find_col = 0;
            }
        }

        img_col += 1; //add last one

        if(debug)
            printf("IMG:\nLin: %d\nCol: %d\n", img_lin, img_col);

    }

    rewind(file);

    int* matrix = calloc(img_lin*img_col, sizeof(int));


    for(int i = 0; i < img_lin; i++){
        for(int j = 0; j < img_col; j++){
            fscanf(file, "%d;", matrix+(i*img_col)+j);
        }
    }

    fclose(file);

    return matrix;
}

char* get_filename(char* path, int id, char* postfix){
// this method will get a base path: "dataset/grass/grass_", and will
// add the id "_id" (from 01 to 50), and a postfix: ".txt"
// Example:
// get_filename("dataset/grass/grass_", 01, ".txt")
// == "dataset/grass/grass_01.txt"

    // allocate space for filename
    char* filename = calloc(
        (strlen(path) + strlen(postfix) + 2), //base path + postfix + 2 for number
        sizeof(char)
    );

    if(filename == NULL)
        exit_with_error("\nError allocating memory for filename!", 1);
    

    strcpy(filename, path);
    
    char buffer[13];
    sprintf(buffer, "%02d", id);

    strcat(filename, buffer);
    strcat(filename, postfix);

    if(debug)
        printf("\nGenerated filename: %s", filename);

    return filename;
}

int** read_files(char* datatype){
    // read all grass files and return a pointer to an array with all of them

    char* path = calloc(255, sizeof(char));

    if(path == NULL)
        exit_with_error("\nMemory allocation error", 1);

    strcpy(path, dataset_path);
    strcat(path, datatype);

    int **read_data = (int**) calloc(50, sizeof(int*));

    if(read_data == NULL)
        exit_with_error("\nMemory allocation error", 1);

    for(int i = 1; i <= 50; i++){
       
        char * filename = get_filename(path, i, ".txt");

        int *mat = read_txt_file(filename);

        read_data[i] = mat;

        free(filename);
    }

    if(debug)
        printf("\nPATH: %s\n", path);


    if(debug)
        puts("Freeing path string");

    free(path);

    return read_data;

}

int contain(int* vec, int len, int number){
    
    int result = 0;

    // TODO: optimize
    for(int i = 0; i < len; i++){
        
        if(vec[i] == number){
            result = 1;
            break;
        }
    }

    return result;
}

void get_random_set(int** ptr, int size, int** set, int** not_set){
// randomly separate a ptr into two set, half the size of the original

    if(debug)
        puts("\nGenerating random number sequence");

    // generate a random number sequence of n itens
    int* random_num = (int*) calloc(size, sizeof(int));

    if(random_num == NULL)
        exit_with_error("\nMemory allocation error", 1);
    
    //start array with -1
    memset(random_num, -1, size);

    for(int i = 0; i < size/2; i++){
        
        int n = rand() % 50; // generate pseudo-random number [0...49]

        if(contain(random_num, 50, n)){
            i--;
            continue;
        }

        random_num[i] = n;
    }

    int j = 25;
    for(int i = 0; i < size; i++){
        if(contain(random_num, 50, i) == 0){
            random_num[j] = i;
            j++;
        }
    }

    if(debug){
        printf("Generated random set:\n[");
        for(int i = 0; i < size; i++){
            printf(" %d ", random_num[i]);
        }
        printf("]\n");
    }

    // with generated numbers, we separete each set (A = rand[itens 0 to 24])
    for(int i = 0; i < 25; i++)
        set[i] = ptr[random_num[i]];
    
    // and we supply a set of not set itens (not A)
    for(int i = 25; i < 50; i++)
        not_set[i-25] = ptr[random_num[i]];

    free(random_num);

}

int main(int argc, char **argv)
{
    // argv[0] = (str) program executable
    // argv[1] = (str) dataset path
    // argv[2] = (int) debug mode
    
    dataset_path = argv[1];

    if(argc > 1){

        // convert debug var from char to int
        char *c = argv[2];
        debug = (int) strtol(c, NULL, 10);

        if(debug){
            printf("Debug: %d\n", debug);
            printf("PATH: %s\n", dataset_path);
        }
    }

    int** grass = read_files("/grass/grass_");
    int** asphalt = read_files("/asphalt/asphalt_");

    if(debug)
        puts("===Allocating memory for sets===");

    // alocating memory for grass sets
    int** grass_learn_set = (int**) calloc(25, sizeof(int*));
    int** grass_test_set = (int**) calloc(25, sizeof(int*));

    if(grass_test_set == NULL || grass_learn_set == NULL)
        exit_with_error("\nMemory allocation error", 1);

    get_random_set(grass, 50, grass_learn_set, grass_test_set);

    // alocating asphalt sets
    int** asphalt_learn_set = (int**) calloc(25, sizeof(int*));
    int** asphalt_test_set = (int**) calloc(25, sizeof(int*));

    if(asphalt_test_set == NULL || asphalt_learn_set == NULL)
        exit_with_error("\nMemory allocation error", 1);

    get_random_set(grass, 50, asphalt_learn_set, asphalt_test_set);

    // TODO: proccess data


    // free memory
    if(debug)
        puts("===Freeing set memory===");

    free_data(grass, 50);
    // since the members of grass where cleaned, we can
    // clean just the set reference 
    free(grass_test_set);
    free(grass_learn_set);

    free_data(asphalt, 50);

    if(debug)
        puts("END");

	return 0;
}
