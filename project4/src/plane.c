#include "plane.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* PLANE_NAMES[] = {"VG3001",
"JJ4404", "LN7001", "TG1501", "GL7602", "TT1010", "AZ1009", "AZ1008",
"AZ1010", "TG1506", "VG3002", "JJ4402", "GL7603", "RL7880", "AL0012",
"TT4544", "TG1505", "VG3003", "JJ4403", "JJ4401", "LN7002", "AZ1002",
"AZ1007", "GL7604", "AZ1006", "TG1503", "AZ1003", "JJ4403", "AZ1001",
"LN7003", "AZ1004", "TG1504", "AZ1005", "TG1502", "GL7601",
"TT4500", "RL7801", "JJ4410", "GL7607", "AL0029", "VV3390", "VV3392",
"GF4681", "GF4690", "AZ1020", "JJ4435", "VG3010", "LF0920", "AZ1065",
"LF0978", "RL7867", "TT4502", "GL7645", "LF0932", "JJ4434", "TG1510",
"TT1020", "AZ1098", "BA2312", "VG3030", "BA2304", "KL5609",
"KL5610", "KL5611"};


char *get_plane_name(Plane *p){
        
    char *str = (char *) calloc(32, sizeof(char));

    
    if(str == NULL){
        puts("\nMemory error - Time string");
        exit(1);
    }
	
    if(p->type == 'A'){
        sprintf(str, "%s-%c-%02d", p->name, p->type, p->fuel < 0 ? 0 : p->fuel);
    }else{
        sprintf(str, "%s-%c", p->name, p->type);
    }
    
    return str;
}


int get_random_fuel(){

    return rand() % 13; //0 to 12

}

char get_random_type(){
    
    int random_num = rand() % 2; //0 to 1

    return random_num ? 'A' : 'D';
}

void remove_fuel(Plane *p){

    if (p->fuel >= 0 && p->type != 'D'){
     
        p->fuel -= 1;
    
    }
}

Plane *create_plane(const char *name, char type, int fuel){

    Plane *p = (Plane *) malloc(sizeof(Plane));

    if (p == NULL){
        printf("\nMemory error - Plane");
        exit(1);
    }

    strcpy(p->name, name);
    
    p->type = type;
    p->fuel = (type == 'A') ? fuel : 12;

    return p;
}

void free_plane(Plane *p){
    
    free(p);
    
}