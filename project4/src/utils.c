#include "utils.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plane.h"
#include "queue.h"


void decrease_all_fuel(Queue *q){

    List *iterator = q->end;

    while(iterator != NULL){

        remove_fuel(iterator->plane);

        iterator = iterator->next;
    }

}

void plane_fall(List *element, Queue *q) {

	puts("\n\nALERTA CRITICO, AERONAVE ");
	print_plane_name(element->plane);
	puts(" IRA CAIR\n\n");

	delete_from_queue(element, q);

}
