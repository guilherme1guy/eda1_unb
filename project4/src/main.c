#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "plane.h"
#include "queue.h"
#include "utils.h"

int global_time = 0;


void update(Queue *plane_queue) {

	int emergency_status = 0;

	// every 10 time units the fuel is decreased
	if (global_time > 0 && global_time % 10 == 0) {
		decrease_all_fuel(plane_queue);
	}

	List *iterator = plane_queue->end;

	while (iterator != NULL) {
		List *current = iterator;
		iterator = iterator->next;

		if (current->plane->fuel < 0) {
			
			plane_fall(current, plane_queue);
		}

	}

	global_time++;

}

int main() {
	
    srand(time(NULL));

	puts("Hello world!\n");

	Queue *plane_queue = create_queue();

	for (int i = 0; i < 64; i++) {
	
		Plane *p = create_plane(PLANE_NAMES[i], get_random_type(), get_random_fuel());

		List *element = create_element(p);

		enqueue(element, plane_queue);
	}

	while (!is_empty(plane_queue)) {
		
		update(plane_queue);
	
	}


	free_queue(plane_queue);

	return 0;
}
