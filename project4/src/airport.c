#include "airport.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int approach_time = 1;
const int land_time = 3;
const int take_off_time = 2;


Airport *start_airport() {

    // 3 lanes, lane 3 is special

    Airport *a = (Airport *) malloc(sizeof(Airport));

    if(a == NULL){
        puts("\nMemory error - Airport");
        exit(1);
    }

    a->lanes = (Lane **) calloc(3, sizeof(Lane *));

    if(a->lanes == NULL){
        puts("\nMemory error - Lanes");
        exit(1);
    }

    a->count = 0;
    
    for (int i = 0; i < 3; i++) {
        
        Lane *l = (Lane *) malloc(sizeof(Lane));

        
        if(l == NULL){
            puts("\nMemory error - Lane");
            exit(1);
        }

        l->id = i;
        l->in_use_for_time = 0;
        l->special = (i == 2);
        
        a->lanes[a->count] = l;
        a->count++;

    }

    return a;

}

void update_airport_lanes(Airport *a){

    for(int i = a->count - 1; i >= 0; i--){

       Lane *l = a->lanes[i];

       if (l->in_use_for_time > 0) {
           l->in_use_for_time--;
        }
    }

}

int can_land(Airport *a, int lane, int emergency_status) {

    Lane *selected_lane = a->lanes[lane];

    // lane 3 can only be used to take off
    if (lane == 2 && !emergency_status){
        return 0;
    }

    return (selected_lane->in_use_for_time <= 0);
}

int can_takeoff(Airport *a, int lane, int emergency_status) {

    Lane *selected_lane = a->lanes[lane];

    return (selected_lane->in_use_for_time <= 0);
}


void free_airport(Airport *a) {

    for(int i = a->count - 1; i >= 0; i--){

        free(a->lanes[i]);
    }

    free(a->lanes);
    free(a);

}


