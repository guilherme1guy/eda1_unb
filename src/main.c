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

void read_grass_files(){
    // read all grass files and return a pointer to an array with all of them

    char* path = calloc(255, sizeof(char));
    strcpy(path, dataset_path);
    strcat(path, "/grass/grass_");

    int **grass = (int**) calloc(1, sizeof(int*));

    for(int i = 1; i <= 50; i++){
       
        char * filename = get_filename(path, i, ".txt");

        int *mat = read_txt_file(filename);

        grass = (int**) realloc(grass, i * sizeof(mat));
    
        free(filename);
    }

    if(debug)
        printf("\nGRAS_PATH: %s\n", path);

    //   for(int i = 0; i < img_lin; i++){
    //     for(int j = 0; j < img_col; j++){
    //         printf(" %d ", *(mat+(i*img_col)+j));
    //         if(j == img_col-1)
    //             printf("\n");
    //     }

    free(path);
    
    for(int i = 0; i < 50; i++){
        free(grass[i]);
    }

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

        printf("Debug: %d\n", debug);
        printf("PATH: %s\n", dataset_path);
    
    }

    read_grass_files();

	return 0;
}
