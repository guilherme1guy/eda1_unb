#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DATATYPE_GRASS "/grass/grass_"
#define DATATYPE_ASPHALT "/asphalt/asphalt_"

int debug = 0;
char *dataset_path;

int img_lin = 0;
int img_col = 0;

void exit_with_error(char* error, int code){
    printf("%s\n", error);
    exit(code);
}


void free_data(int*** ptr){

    if(debug)
        printf("\nFreeing members of **ptr: %p\n", *ptr);
    
    for(int i = 0; *(*ptr + i) != NULL; i++){
       // if(debug)
       //    printf("\nFreeing ptr[%d]: %p\n", i, *(*ptr + i));

        free(*(*ptr + i));
    }

    if(debug)
        printf("\nFreeing **ptr: %p\n", *ptr);
    
    free(*ptr);

    if(debug)
        printf("\nFREE");
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

int* calculate_ILBP_for_marixt(int* mat, int lin, int col, int max_gray_level){
    
    int* ilbp = (int *) calloc(max_gray_level, sizeof(int));

    if(ilbp == NULL)
        exit_with_error("Error alocating memory", 1);

    for(int i = 0; i < lin; i++){
        for(int j = 0; j < col; j++){

            int sub_matrix[3][3];

            // separate sub-matrix acording to rules
            // we will consider the outside of main matrix as 0

            // top line
            sub_matrix[0][0] = (i == 0 || j == 0) ? 0 : *(mat + ((i - 1) * col) + (j - 1));
            sub_matrix[0][1] = (i == 0) ? 0 : *(mat + ((i - 1) * col) + j);
            sub_matrix[0][2] = (i == 0 || j == (col - 1)) ? 0 : *(mat + ((i - 1) * col) + (j + 1));

            // center line
            sub_matrix[1][0] = (j == 0) ? 0 : *(mat + (i * col) + (j - 1));
            sub_matrix[1][1] = *(mat + (i * col) + j); // center piece 
            sub_matrix[1][2] = (j == (col - 1)) ? 0 : *(mat + (i * col) + (j + 1));

            // botton line
            sub_matrix[2][0] = (i == (lin - 1) || j == 0) ? 0 : *(mat + ((i + 1) * col) + (j - 1));
            sub_matrix[2][1] = (i == (lin - 1)) ? 0 : *(mat + ((i + 1) * col) + j);
            sub_matrix[2][2] = (i == (lin - 1) || j == (col - 1)) ? 0 : *(mat + ((i + 1) * col) + (j + 1));

            // calculate the average value of the sub-matrix 
            double avg = 0;
            for(int x = 0; x < 3; x++){
                for(int y = 0; y < 3; y++){
                    avg += sub_matrix[x][y];
                }
            }

            avg = avg/9;

            // find the lowest value in binary
            
            // a unsigned short has 16 bits (2 bytes)
            unsigned short bin = 0; //0000 0000 0000 0000
            
            // find the initial value before shifts to determine lowest
            // loop should do the following path: x,y -> 0,0 -> 0,1 -> 0,2 -> 1,2 -> 2,2 -> 2,1 -> 2,0 -> 1,0 -> 1,1 
            int x = 0, y = 0;
            while(1){
                bin =  bin << 1; // shift bin to the left by 1 bit
                                
                if(sub_matrix[x][y] >= avg){
                    bin = bin | 0x0001; // bitwise OR with 0000 0000 0000 0001 (to insert a 1 on the end)
                } // there is no need to insert a 0      

                
                if(x == 1 && y == 1){
                    break;
                }

                if((x == 0 || x == 1) && y != 2){
                    y++;
                    continue;
                }

                if(y == 2 && x != 2){
                    x++;
                    y = 2;
                    continue;
                }

                if(x == 2 && y != 0){
                    x = 2;
                    y--;
                    continue;
                }else{
                    x = 1;
                    y = 0;
                }
            }
            
            // now we have the bit representing the current value
            // we want to make it rotation safe, to do it we will shift the binary
            // and find the lowest value

            // NOTE: unsigned short (unsigned, 16bits) to int (signed, 32bits) preserves value
            int lowest_bin_num = bin; // initialize with bin

            for(int k = 0; k < 9; k++){
                
                unsigned short extracted = bin & 0x0100; // 0x0010 = 0000 0001 0000 0000
                extracted = extracted >> 8; // shift by 8 => 0000 000X 0000 0000 >> 0000 0000 0000 000X
                
                bin = bin << 1; // shift bin to left by 1 bit

                bin = bin | extracted; // insert the extracted bit on the end on bin
                bin = bin & 0x01FF; // clean the unused part, 0x01FF = 0000 0001 1111 1111 

                if(bin < lowest_bin_num)
                    lowest_bin_num = bin;                 

            }
            // printf("HERE\n");
            printf("%d %d\n", lowest_bin_num, ilbp[lowest_bin_num]);

            // *(ilbp + (i * col) + j) = lowest_bin_num; // save lowest value from rotations into ilbp matrix 
            *(ilbp + lowest_bin_num ) += 1;
        }
    } 

    return ilbp;
}

int** read_files(char* datatype){
    // read all txt files and return a pointer to an array with the normalized frequence vector
    // of each image

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

        int *ilbp = calculate_ILBP_for_marixt(mat, img_lin, img_col, 255);

        *(read_data + i - 1) = ilbp;

        free(mat);
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
    // for the current use case, we can afford to use a linear search
    for(int i = 0; i < len; i++){
        
        if(vec[i] == number){
            result = 1;
            break;
        }
    }

    return result;
}

void get_random_set(int*** ptr, int size, int*** set, int*** not_set){
// randomly separate a ptr into two set, half the size of the original

    if(debug)
        puts("\nGenerating random number sequence");

    // generate a random number sequence of n itens
    srand(time(NULL)); 
    int* random_num = (int*) calloc(size, sizeof(int));

    if(random_num == NULL)
        exit_with_error("\nMemory allocation error", 1);
    
    //start array with -1
    memset(random_num, -1, size * sizeof(int));

    // randomly generate first positions
    for(int i = 0; i < size/2; i++){
        
        int n = rand() % size; // generate pseudo-random number [0...size]

        if(contain(random_num, size, n)){
            i--;
            continue;
        }

        random_num[i] = n;
    }

    // after half of the random num generation, we just complete with what is missing
    int j = size/2;
    for(int i = 0; i < size; i++){
        if(contain(random_num, size, i) == 0){
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

    // with generated numbers, we separete each set (A = rand[itens 0 to size/2])
    for(int i = 0; i < size/2; i++)
        *(*set + i) = *(*ptr + (random_num[i]));
    
    // and we supply a set of not set itens (not A)
    //for(int i = size/2; i < size; i++)
    //    not_set[i - (size/2)] = ptr[random_num[i]];

    free(random_num);

}

void get_ILBP_set_for_data_type(char* datatype, int set_size, int*** full_set, int*** learn_set,  int*** test_set ){
// receive a datatype and places where to store data, read data from disk, calc ILBP
// and save into pointers
// NOTE: pass NULL pointers

    if(debug)
        printf("\nPreparing ILBP for %s datatype\nReading files...\n", datatype);

    // read files and convert to ILBP
    *full_set = read_files(datatype);

    if(debug)
        printf("\nDatatype: %s\nSeparating read data into sets", datatype);

    // alocating memory for sets
    *learn_set = (int**) calloc(set_size/2, sizeof(int*));
    *test_set = (int**) calloc(set_size/2, sizeof(int*));

    if(test_set == NULL || learn_set == NULL)
        exit_with_error("\nMemory allocation error", 1);
}

int main(int argc, char **argv)
{
    // argv[0] = (str) program executable
    // argv[1] = (str) dataset path
    // argv[2] = (int) debug mode
    
    dataset_path = argv[1];

    if(argc > 2){

        // convert debug var from char to int
        char *c = argv[2];
        debug = (int) strtol(c, NULL, 10);

        if(debug){
            printf("Debug: %d\n", debug);
            printf("PATH: %s\n", dataset_path);
        }
    }

    int** grass;
    int** grass_learn_set;
    int** grass_test_set;

    get_ILBP_set_for_data_type(DATATYPE_GRASS, 50, &grass, &grass_learn_set, &grass_test_set);

    // separate full_set into a learn_set and a test set
    get_random_set(&grass, 50, &grass_learn_set, &grass_test_set);

    int** asphalt;
    int** asphalt_learn_set;
    int** asphalt_test_set;
    get_ILBP_set_for_data_type(DATATYPE_ASPHALT, 50, &asphalt, &asphalt_learn_set, &asphalt_test_set);

    get_random_set(&asphalt, 50, &asphalt_learn_set, &asphalt_test_set);

    // TODO: proccess data

    // free memory
    if(debug)
        printf("\n===Freeing grass sets memory===\ngrass = %p\ngrass_learn = %p\ngrass_test = %p\n", grass, grass_learn_set, grass_test_set);

    free_data(&grass);
    // since the members of grass where cleaned, we can
    // clean just the set reference 
    free(grass_test_set);
    free(grass_learn_set);

    if(debug)
        printf("\n===Freeing asphalt sets memory===\nasphalt = %p\nasphalt_learn = %p\nasphalt_test = %p\n", asphalt, asphalt_learn_set, asphalt_test_set);

    free_data(&asphalt);
    free(asphalt_learn_set);
    free(asphalt_test_set);

    if(debug)
        puts("\nEND");

	return 0;
}
