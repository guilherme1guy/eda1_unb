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

	int tries = 0;
	for(int i = 0; i < 3; i++) {

		List *element = dequeue(plane_queue);

		if (element == NULL) {
			break;
		}

		if (is_empty(plane_queue)) {
			break;
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

		if(changed){
			free_element(element);
		}else{
			
			if(p->fuel == 0){
			
				enqueue_start(element, plane_queue);
			
			}else{
			
				enqueue(element, plane_queue);	
			
			}
			
			i--;
			tries++;

			if(tries == 3){
				break;
			}
				
		}
	}

	List *iterator = plane_queue->end;

	while (iterator != NULL) {
		List *current = iterator;

		iterator = iterator->next;

		if (current->plane->fuel < 0) {
			
			plane_fall(current, plane_queue);
			planes_fallen++;
		}
	}

	global_time++;

}

int main() {
	
	srand(time(NULL));

	Queue *plane_queue = create_queue();
	Airport *airport = start_airport();

	for (int i = 0; i < 64; i++) {
	
		Plane *p = create_plane(PLANE_NAMES[i], get_random_type(), get_random_fuel());

		List *element = create_element(p);

		enqueue(element, plane_queue);
		planes_made++;
	}

	while (!is_empty(plane_queue)) {
		
		update(plane_queue, airport);
	}

	printf("\n\n=========\nResumo:\n\tCriados: %d\n\tPousos: %d\n\tDecolagens: %d\n\tQuedas: %d\n\t", planes_made, planes_landed, planes_takeoff, planes_fallen);

	free_queue(plane_queue);
	free_airport(airport);

	return 0;
}
