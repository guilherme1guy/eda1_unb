#include "utils.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plane.h"
#include "queue.h"
#include "airport.h"

const int initial_time[] = {8, 0};
const int timestep_size = 5;

void decrease_all_fuel(Queue *q){

    List *iterator = q->end;

    while(iterator != NULL){

        remove_fuel(iterator->plane);

        iterator = iterator->next;
    }

}

void plane_fall(List *element, Queue *q) {

    char *plane_code = get_plane_name(element->plane);

	printf("\n\nALERTA CRITICO, AERONAVE %s IRA CAIR\n\n", plane_code);

    free(plane_code);

	delete_from_queue(element, q);

}

void print_queue(Queue *q){

    List *iterator = q->end;

    while(iterator != NULL){

        char *plane_code = get_plane_name(iterator->plane);

        printf("\n[%s]", plane_code);

        free(plane_code);

        iterator = iterator->next;

    }

}

void plane_action(Plane *p, int lane, int global_time){
    
    char *current_time = get_time_from_start(global_time); 
    char *plane_code = get_plane_name(p);
    
	printf("\n\nCodigo do voo: %s", plane_code);
	printf("\nStatus: aeronave %s", (p->type == 'D') ? "decolou" : "pousou");
	printf(" \nHorario do inicio do procedimento: %s", current_time);
	printf("\nNumero da pista : %d\n\n", lane + 1);

    free(current_time);
    free(plane_code);

}

void land_plane(Airport *a, Plane *p, int lane, int global_time){

    Lane *l = a->lanes[lane];

    l->in_use_for_time = land_time + approach_time;

    plane_action(p, lane, global_time);

}

void takeoff_plane(Airport *a, Plane *p, int lane, int global_time){
    
    Lane *l = a->lanes[lane];

    l->in_use_for_time = take_off_time;

    plane_action(p, lane, global_time);
   
}

char *get_time_from_start(int timesteps){
    
    int hours = initial_time[0];
    int minutes = initial_time[1];

    // 12 steps == 1 hour

    hours += (timesteps * timestep_size) / 60;

    while (hours >= 24){
        hours -= 24;
    }

    minutes = timesteps * timestep_size;

    while (minutes >= 60){
        minutes -= 60;
    }

    char *str = (char *) calloc(32, sizeof(char));

    
    if(str == NULL){
        puts("\nMemory error - Time string");
        exit(1);
    }
	
    sprintf(str, "%02d:%02d", hours, minutes);

    return str;
}

int has_emergency(Queue *q){

    List *queue_start = q->start;
    
    if (queue_start == NULL) return 0;
    
    List *second = find_previous_element(q->start, q);

    if (second == NULL) return 0;

    List *third = find_previous_element(second, q);

    if (third == NULL) return 0;

    return (
        queue_start->plane->fuel == 0 &&
        second->plane->fuel == 0 &&
        third->plane->fuel == 0
    );

}
