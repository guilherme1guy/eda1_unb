#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define DATATYPE_GRASS "/grass/grass_"
#define DATATYPE_ASPHALT "/asphalt/asphalt_"

#define MAX_GRAY_LEVEL 256
#define ILBP_MAX 512
#define NUMBER_OF_METRICS 24

int debug = 0;
char *dataset_path;

int img_lin = 0;
int img_col = 0;

double hits = 0;
double false_positive = 0;
double false_negative = 0;

void exit_with_error(char* error, int code) {
	printf("%s\n", error);
	exit(code);
}


void free_data(double*** ptr) {

	if (debug)
		printf("\nFreeing members of **ptr: %p\n", *ptr);

	for (int i = 0; *(*ptr + i) != NULL; i++) {
		// if(debug)
		//    printf("\nFreeing ptr[%d]: %p\n", i, *(*ptr + i));

		free(*(*ptr + i));
	}

	if (debug)
		printf("\nFreeing **ptr: %p\n", *ptr);

	free(*ptr);

	if (debug)
		printf("\nFREE");
}

int* read_txt_file(char* filename) {
	// open a file from dataset indicaed by filename, read its contents
	// and returns a matrix with it


	FILE* file = fopen(filename, "r");

	if (file == NULL) {
		char error[255] = "Cannot open file: ";
		strcat(error, filename);
		exit_with_error(error, 1);
	}

	// all the files have the same length of data, so we only
	// need to find the size for the first image/txt

	if (img_lin == 0) {
		// find the size of images

		char ch = 0;

		int find_col = 1;

		while (feof(file) == 0) {

			ch = fgetc(file);

			// we will user the ; char to find the aumont of
			// data per line
			if (find_col && ch == ';')
				img_col++;

			if (ch == '\n') {
				img_lin++;
				find_col = 0;
			}
		}

		img_col += 1; //add last one

		if (debug)
			printf("\nIMG:\nLin: %d\nCol: %d\n", img_lin, img_col);

	}

	rewind(file);

	int* matrix = calloc(img_lin*img_col, sizeof(int));


	for (int i = 0; i < img_lin; i++) {
		for (int j = 0; j < img_col; j++) {
			fscanf(file, "%d;", matrix + (i*img_col) + j);
		}
	}

	fclose(file);

	return matrix;
}

char* get_filename(char* path, int id, char* postfix) {
	// this method will get a base path: "dataset/grass/grass_", and will
	// add the id "_id" (from 01 to 50), and a postfix: ".txt"
	// Example:
	// get_filename("dataset/grass/grass_", 01, ".txt")
	// == "dataset/grass/grass_01.txt"

	// allocate space for filename
	char* filename = calloc(
		(strlen(path) + strlen(postfix) + 12), //base path + postfix + 12 for buffer
		sizeof(char)
	);

	if (filename == NULL)
		exit_with_error("\nError allocating memory for filename!", 1);


	strcpy(filename, path);

	char buffer[12] = { 0 };
	sprintf(buffer, "%02d", id);

	strcat(filename, buffer);
	strcat(filename, postfix);

	if (debug)
		printf("\nGenerated filename: %s", filename);

	return filename;
}

double* get_glcm_for_direction(int direction[2], int* mat, int lin, int col, int max_gray_level) {
	// get a direction and calculate glcm values for that direction

	// direction matrix:
	// [x,y]    [0,0][0,1][0,2]     []     [UP]     []
	// [x,y]    [1,0][1,1][1,2]     [LEFT] [0]      [RIGHT]
	// [x,y]    [2,0][2,1][2,2]     []     [BOTTON] []

	// filter direction to convert it from having a center in 1,1
	// to having a center in 0,0 (because math...)
	direction[0] -= 1;
	direction[1] -= 1;

	double contrast = 0;
	double energy = 0;
	double homogeneity = 0;

	// create referential GLCM matrix
	double* glcm_matrix = (double *)calloc(pow(max_gray_level, 2), sizeof(double));

	if (glcm_matrix == NULL)
		exit_with_error("Failed to allocate memory", 1);

	// i,j are for the referential matrix (glcm_matrix), the value on
	// a position i,j represent the following:
	// "How many times does J happens on DIRECTION of I?" (beeing direction defined above)

	// this loop scans the img matrix searching for instances of:
	// mat[x][y] == j and mat[X,Y on DIRECTION] == i and save the value on glcm referential matrix
	for (int x = 0; x < lin; x++) {
		for (int y = 0; y < col; y++) {

			int j = mat[(x*col) + y]; // the position where we are testing
									  // we need apply the direction to test if there is a
									  // i in the DIRECTION form j

									  // how to apply DIRECTION? I DON'T KNOW!!! (yet)

									  // check if we are inside tha matrix bounds
			if (
				x + direction[0] < 0 || x + direction[0] >= lin
				||
				y + direction[1] < 0 || y + direction[1] >= col
				) {
				// we are unable to check if mat[x][y] == j and mat[X,Y on DIRECTION] == i
				// when mat[x,y on DIRECTION] is outside the matrix
				continue;
			}

			// this should apply the DIRECTION
			int i = mat[(x + direction[0])*col + (y + direction[1])];

			glcm_matrix[(i*max_gray_level) + j] += 1;
		}
	}

	for (int i = 0; i < max_gray_level; i++) {
		for (int j = 0; j < max_gray_level; j++) {

			double current_value = glcm_matrix[(i*max_gray_level) + j];

			contrast += pow(fabs(i - j), 2) * current_value;
			energy += pow(current_value, 2);
			homogeneity += current_value / (1 + abs(i - j));
		}
	}

	double *metrics = calloc(3, sizeof(double));

	metrics[0] = contrast;
	metrics[1] = energy;
	metrics[2] = homogeneity;

	free(glcm_matrix);

	return metrics;
}

double* calculate_GLCM_for_matrix(int* mat, int lin, int col, int max_gray_level) {
	// calculate contrast, energy and homogeneity for a given matrix
	// on all 8 possible directions and return a vector containing all the 24 metrics
	// metric order on vector: contrast, energy, homogeneity

	double *glcm = calloc(24, sizeof(double));

	// considerating the following pattern:
	// direction matrix:
	// [x,y]    [0,0][0,1][0,2]     []     [UP]     []
	// [x,y]    [1,0][1,1][1,2]     [LEFT] [0]      [RIGHT]
	// [x,y]    [2,0][2,1][2,2]     []     [BOTTON] []
	// then [0,1] is a vector pointing upwards, [2,1] points to botton and etc...
	int k = 0;
	for (int direction_x = 0; direction_x < 3; direction_x++) {
		for (int direction_y = 0; direction_y < 3; direction_y++) {
			if (direction_x == 1 && direction_y == 1) {
				continue; // skip unwanted direction
			}

			int vec[2] = { direction_x, direction_y };

			double *metrics = get_glcm_for_direction(vec, mat, img_lin, img_col, max_gray_level);

			for (int i = 0; i < 3; i++) {
				glcm[k] = metrics[i];
				k++;
			}

			free(metrics);
		}
	}

	return glcm;

}

int* calculate_ILBP_for_marixt(int* mat, int lin, int col) {

	// the ILBP recieves 9 bits, the max value that will be saved is
	// 2^9 - 1
	int* ilbp = (int *)calloc(512, sizeof(int));

	if (ilbp == NULL)
		exit_with_error("Error alocating memory", 1);

	for (int i = 0; i < lin; i++) {
		for (int j = 0; j < col; j++) {

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
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					avg += sub_matrix[x][y];
				}
			}

			avg = avg / 9;

			// find the lowest value in binary

			// a unsigned short has 16 bits (2 bytes)
			unsigned short bin = 0; //0000 0000 0000 0000

									// find the initial value before shifts to determine lowest
									// loop should do the following path: x,y -> 0,0 -> 0,1 -> 0,2 -> 1,2 -> 2,2 -> 2,1 -> 2,0 -> 1,0 -> 1,1
			int x = 0, y = 0;
			while (1) {
				bin = bin << 1; // shift bin to the left by 1 bit

				if (sub_matrix[x][y] >= avg) {
					bin = bin | 0x0001; // bitwise OR with 0000 0000 0000 0001 (to insert a 1 on the end)
				} // there is no need to insert a 0


				if (x == 1 && y == 1) {
					break;
				}

				if ((x == 0 || x == 1) && y != 2) {
					y++;
					continue;
				}

				if (y == 2 && x != 2) {
					x++;
					y = 2;
					continue;
				}

				if (x == 2 && y != 0) {
					x = 2;
					y--;
					continue;
				}
				else {
					x = 1;
					y = 0;
				}
			}

			// now we have the bit representing the current value
			// we want to make it rotation safe, to do it we will shift the binary
			// and find the lowest value

			// NOTE: unsigned short (unsigned, 16bits) to int (signed, 32bits) preserves value
			int lowest_bin_num = bin; // initialize with bin

			for (int k = 0; k < 9; k++) {

				unsigned short extracted = bin & 0x0100; // 0x0010 = 0000 0001 0000 0000
				extracted = extracted >> 8; // shift by 8 => 0000 000X 0000 0000 >> 0000 0000 0000 000X

				bin = bin << 1; // shift bin to left by 1 bit

				bin = bin | extracted; // insert the extracted bit on the end on bin
				bin = bin & 0x01FF; // clean the unused part, 0x01FF = 0000 0001 1111 1111

				if (bin < lowest_bin_num)
					lowest_bin_num = bin;

			}

			*(ilbp + lowest_bin_num) += 1;
		}
	}

	return ilbp;
}

double max_element(double* vector, int len){

    double max = vector[0];

    for(int i = 0; i < len; i++){
        if(vector[i] > max){
            max = vector[i];
        }
    }

    return max;
}

double min_element(double* vector, int len){

    double min = vector[0];

    for(int i = 0; i < len; i++){
        if(vector[i] < min){
            min = vector[i];
        }
    }

    return min;
}

void normalize_vector(double** vector, int len){

    double max_value = max_element(*vector, len);
    double min_value = min_element(*vector, len);

    for(int i = 0; i < len; i++){
        *(*vector + i) = ( *(*vector + i) - min_value ) / (max_value - min_value);
    }
} 

double** read_files(char* datatype) {
	// read all txt files and return a pointer to an array with the normalized frequence vector
	// of each image

	char* path = calloc(255, sizeof(char));

	if (path == NULL)
		exit_with_error("\nMemory allocation error", 1);

	strcpy(path, dataset_path);
	strcat(path, datatype);

	double **read_data = (double**)calloc(50, sizeof(double*));

	if (read_data == NULL)
		exit_with_error("\nMemory allocation error", 1);

	for (int i = 1; i <= 50; i++) {

		int ilbp_max_num = ILBP_MAX;

		char *filename = get_filename(path, i, ".txt");

		int *mat = read_txt_file(filename);

		int *ilbp = calculate_ILBP_for_marixt(mat, img_lin, img_col); //return vec[512]

		double *glcm = calculate_GLCM_for_matrix(mat, img_lin, img_col, MAX_GRAY_LEVEL); // return vec[24]

		double *img_descriptor = calloc((ilbp_max_num + NUMBER_OF_METRICS), sizeof(double)); // concatenate ilbp and glcm

		for (int j = 0; j < (ilbp_max_num + 24); j++) {

			img_descriptor[j] = (j < ilbp_max_num) ? (double)ilbp[j] : glcm[j - ilbp_max_num];
		}

		normalize_vector(&img_descriptor, (ilbp_max_num + 24));

		*(read_data + i - 1) = img_descriptor;

		free(mat);
		free(ilbp);
		free(glcm);
		free(filename);
		
	}

	if (debug)
		printf("\nPATH: %s\n", path);


	if (debug)
		puts("Freeing path string");

	free(path);

	return read_data;

}

int contain(int* vec, int len, int number) {

	int result = 0;

	// TODO: optimize
	// for the current use case, we can afford to use a linear search
	for (int i = 0; i < len; i++) {

		if (vec[i] == number) {
			result = 1;
			break;
		}
	}

	return result;
}

void get_random_set(double*** ptr, int size, double*** set, double*** not_set) {
	// randomly separate a ptr into two set, half the size of the original

	if (debug)
		puts("\nGenerating random number sequence");

	// generate a random number sequence of n itens
	srand(time(NULL));
	int* random_num = (int*)calloc(size, sizeof(int));

	if (random_num == NULL)
		exit_with_error("\nMemory allocation error", 1);

	//start array with -1
	memset(random_num, -1, size * sizeof(int));

	// randomly generate first positions
	for (int i = 0; i < size / 2; i++) {

		int n = rand() % size; // generate pseudo-random number [0...size]

		if (contain(random_num, size, n)) {
			i--;
			continue;
		}

		random_num[i] = n;
	}

	// after half of the random num generation, we just complete with what is missing
	int j = size / 2;
	for (int i = 0; i < size; i++) {
		if (contain(random_num, size, i) == 0) {
			random_num[j] = i;
			j++;
		}
	}

	if (debug) {
		printf("Generated random set:\n[");
		for (int i = 0; i < size; i++) {
			printf(" %d ", random_num[i]);
		}
		printf("]\n");
	}

	// with generated numbers, we separete each set (A = rand[itens 0 to size/2])
	for (int i = 0; i < size / 2; i++)
		*(*set + i) = *(*ptr + (random_num[i]));

	// and we supply a set of not set itens (not A)
	for (int i = size / 2; i < size; i++)
		*(*not_set + i - (size / 2)) = *(*ptr + random_num[i]);

	free(random_num);

}

void get_image_descriptor_set_for_data_type(char* datatype, int set_size, double*** full_set, double*** learn_set, double*** test_set) {
	// receive a datatype and places where to store data, read data from disk, calc ILBP
	// and save into pointers
	// NOTE: pass NULL pointers

	if (debug)
		printf("\nPreparing ILBP for %s datatype\nReading files...\n", datatype);

	// read files and convert to ILBP
	*full_set = read_files(datatype);

	if (debug)
		printf("\nDatatype: %s\nSeparating read data into sets", datatype);

	// alocating memory for sets
	*learn_set = (double**)calloc(set_size / 2, sizeof(double*));
	*test_set = (double**)calloc(set_size / 2, sizeof(double*));

	if (test_set == NULL || learn_set == NULL)
		exit_with_error("\nMemory allocation error", 1);
}

void learn(double** learn_set, double** vector, int individual_vector_size, int number_of_vectors){
    
    for(int i = 0; i < individual_vector_size; i++){
        for(int j = 0; j < number_of_vectors; j++){
            *(*vector + i) += *(*(learn_set + j) + i);
        }

       *(*vector + i) /= (number_of_vectors);
    }
}

void test_descriptor(double* asphalt_descriptor, double* grass_descriptor, double** test_set, int is_grass){

    // calculating for test set
    for(int i = 0; i < 25; i++){

        // i == descriptor from test set
        // j == elements of descriptor

        double distance_from_grass = 0;
        double distance_from_asphalt = 0;

        // get distance from descriptors
        for(int j = 0; j < 25; j++){
            distance_from_asphalt  += pow(asphalt_descriptor[j] - *(*(test_set + i) + j), 2);
            distance_from_grass  += pow(grass_descriptor[j] - *(*(test_set + i) + j), 2);
        }

        distance_from_asphalt = sqrt(distance_from_asphalt);
        distance_from_grass = sqrt(distance_from_grass);
        
        // 0 = asphalt, 1 = grass        
        int test_result = (distance_from_grass < distance_from_asphalt) ? 1 : 0; 
    
        if(test_result == is_grass){
            hits++;
        }else{
            
            // false positive == asphalt identified as grass
            // false negative == grass identified as asphalt
            is_grass ? false_negative++ : false_positive++;
        }
    }

}

int main(int argc, char **argv) {
	// argv[0] = (str) program executable
	// argv[1] = (str) dataset path
	// argv[2] = (int) debug mode

	dataset_path = argv[1];

	if (argc > 2) {

		// convert debug var from char to int
		char *c = argv[2];
		debug = (int)strtol(c, NULL, 10);

		if (debug) {
			printf("Debug: %d\n", debug);
			printf("PATH: %s\n", dataset_path);
		}
	}

	double** grass;
	double** grass_learn_set;
	double** grass_test_set;

    double* grass_descriptor;

	get_image_descriptor_set_for_data_type(DATATYPE_GRASS, 50, &grass, &grass_learn_set, &grass_test_set);

	// separate full_set into a learn_set and a test set
	get_random_set(&grass, 50, &grass_learn_set, &grass_test_set);

    grass_descriptor = calloc(ILBP_MAX + NUMBER_OF_METRICS, sizeof(double));

    learn(grass_learn_set, &grass_descriptor, (ILBP_MAX + NUMBER_OF_METRICS), 25);

    if (debug) {
		printf("Grass descriptor:\n[");
		for (int i = 0; i < ILBP_MAX + NUMBER_OF_METRICS; i++) {
			printf(" %.5lf ", grass_descriptor[i]);
		}
		printf("]\n");
	}

	double** asphalt;
	double** asphalt_learn_set;
	double** asphalt_test_set;
    
    double* asphalt_descriptor;

	get_image_descriptor_set_for_data_type(DATATYPE_ASPHALT, 50, &asphalt, &asphalt_learn_set, &asphalt_test_set);

	get_random_set(&asphalt, 50, &asphalt_learn_set, &asphalt_test_set);

    asphalt_descriptor = calloc(ILBP_MAX + NUMBER_OF_METRICS, sizeof(double));

    learn(asphalt_learn_set, &asphalt_descriptor, (ILBP_MAX + NUMBER_OF_METRICS), 25);

    if (debug) {
		printf("Asphalt descriptor:\n[");
		for (int i = 0; i < ILBP_MAX + NUMBER_OF_METRICS; i++) {
			printf(" %.5lf ", asphalt_descriptor[i]);
		}
		printf("]\n");
	}

    //test_descriptor(double* asphalt_descriptor, double* grass_descriptor, double** test_set, int is_grass)

    test_descriptor(asphalt_descriptor, grass_descriptor, grass_test_set, 1);
    test_descriptor(asphalt_descriptor, grass_descriptor, asphalt_test_set, 0);

    // convert number to porcentage
    hits = (hits * 100)/50.0;
    false_negative = (false_negative * 100)/50.0;
    false_positive = (false_positive *100)/50.0;

    printf("Acertos: %.0lf%%\nFalso positivo: %.0lf%%\nFalso negativo: %.0lf%%\n", hits, false_positive, false_negative);
    

	// free memory
	if (debug)
		printf("\n===Freeing grass sets memory===\ngrass = %p\ngrass_learn = %p\ngrass_test = %p\n", grass, grass_learn_set, grass_test_set);

	free_data(&grass);
	// since the members of grass where cleaned, we can
	// clean just the set reference
	free(grass_test_set);
	free(grass_learn_set);
    free(grass_descriptor);

	if (debug)
		printf("\n===Freeing asphalt sets memory===\nasphalt = %p\nasphalt_learn = %p\nasphalt_test = %p\n", asphalt, asphalt_learn_set, asphalt_test_set);

	free_data(&asphalt);
	free(asphalt_learn_set);
	free(asphalt_test_set);
    free(asphalt_descriptor);

	if (debug) {
		puts("\nEND");
	}

	return 0;
}
