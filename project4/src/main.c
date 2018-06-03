#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "plane.h"
#include "queue.h"
#include "utils.h"
#include "airport.h"

int global_time = 0;

int planes_made = 0;
int planes_landed = 0;
int planes_takeoff = 0;
int planes_fallen = 0;

void update(Queue *plane_queue, Airport *airport) {

	int emergency_status = has_emergency(plane_queue);

	if (emergency_status){
		puts("\n\nALERTA GERAL DE DESVIO DE AERONAVE\n\n");	
	}

	update_airport_lanes(airport);

	// every 10 time units the fuel is decreased
	if (global_time > 0 && global_time % 10 == 0) {
		decrease_all_fuel(plane_queue);
	}


	for(int i = 0; i < 3; i++) {


		List *element = dequeue(plane_queue);

		if (element == NULL) {
			return;
		}

		Plane *p = element->plane;

		int changed = 0;

		if (p->type == 'A' && can_land(airport, i, emergency_status)){
			land_plane(airport, p, i, global_time);
			planes_landed++;
			changed = 1;
		}

		if (p->type == 'D' && can_takeoff(airport, i, emergency_status)){
			takeoff_plane(airport, p, i, global_time);
			planes_takeoff++;
			changed = 1;
		}


		if (changed) {
			free_element(element);
		}
		else {

			if (p->fuel == 0) {

				enqueue_start(element, plane_queue);

			}
			else {

				enqueue(element, plane_queue);

			}
		}
	}



	List *iterator = plane_queue->end;

	char *moved_names[64];
	int moves = 0;

	while (iterator != NULL && !is_empty(plane_queue)) {		
		List *current = iterator;

		iterator = iterator->next;

		if (current->plane->fuel < 0) {
			
			plane_fall(current, plane_queue);
			planes_fallen++;

		} else if (current->plane->fuel == 0 && current != plane_queue->start) {

			int contains = 0;
			for (int i = 0; i < moves; i++) {
				if (strcmp(moved_names[i], current->plane->name) == 0) {
					contains = 1;
					break;
				}
			}

			if (!contains) {

				moved_names[moves] = (char *) calloc(strlen(current->plane->name) + 1, sizeof(char));

				if (moved_names[moves] == NULL) {
					printf("Allocation error - PLANE NAMES");
					exit(1);
				}

				strcpy(moved_names[moves], current->plane->name);
				moves++;

				move_to_start(current, plane_queue);
			}
		}
	}

	for (int i = moves - 1; i >= 0; i--) {
		free(moved_names[i]);
	}

	global_time++;

}

int main() {
	
	unsigned int now = time(NULL);
	srand(now);

	Queue *plane_queue = create_queue();
	Airport *airport = start_airport();

	int takeoffs = 10 + (rand() % (32 - 10));
	int arrivals = 10 + (rand() % (32 - 10));
	int plane_number = takeoffs + arrivals;; 
	

	for (int i = 0; i < plane_number; i++) {

		char type = get_random_type();

		takeoffs <= 0 ? type = 'A' : 0;
		arrivals <= 0 ? type = 'D' : 0;

		type == 'D' ? takeoffs-- : arrivals--;

		Plane *p = create_plane(get_random_plane_name(), type, get_random_fuel());

		List *element = create_element(p);

		enqueue(element, plane_queue);
		planes_made++;
	}

	while (!is_empty(plane_queue)) {
		
		update(plane_queue, airport);
	}

	printf("\n\n=========\nResumo:\n\tCriados: %d\n\tPousos: %d\n\tDecolagens: %d\n\tQuedas: %d\n\tTotal: %d\n\n", planes_made, planes_landed, planes_takeoff, planes_fallen, (planes_landed + planes_takeoff + planes_fallen));

	free_queue(plane_queue);
	free_airport(airport);

	return 0;
}
