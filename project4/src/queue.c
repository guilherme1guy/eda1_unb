#include "queue.h"
#include "plane.h"

#include <stdio.h>
#include <stdlib.h>


List *create_element(Plane *p){

    List *element = (List *) malloc(sizeof(List));

    if(element == NULL){
        printf("\nMemory error - List Element");
        exit(1);
    }

    element->plane = p;
    element->next = NULL;

    return element;
}

Queue *create_queue(){

    Queue *q = (Queue *) malloc(sizeof(Queue));

    if(q == NULL){
        printf("\nMemory error - Queue");
		exit(1);
	}

    q->start = NULL;
    q->end = NULL;

    return q;
}

void  enqueue(List *element, Queue *q){
    
    if (is_empty(q)) {

        q->start = element;
        q->end = element;

    }else{

		if (q->start == q->end) {
		} else {

		}
        element->next = q->end;
        q->end = element;

    }
}

void enqueue_start(List *element, Queue *q){
    
    element->next = NULL;

    if (is_empty(q)) {
        
        enqueue(element, q);
    
    }else{
        
        if(q->start == NULL){
            recover_start(q);
        }

        q->start->next = element;
        q->start = element;

    }

}

void recover_start(Queue *q){
    
    if (q->start == NULL){
		
        List *itr = q->end;

        while(itr->next != NULL){
            
            itr = itr->next;
        }

        q->start = itr;

	}
}

List *dequeue(Queue *q){

    if (is_empty(q)){

        return NULL;

    }else if (q->start == NULL){
		
        recover_start(q);
       
	}

    List *element = q->start;

    List *new_start = find_previous_element(element, q);

	if (new_start != NULL) {
		
		new_start->next = NULL;
	
	} else {
	
		q->end = NULL;

	}
    
    
    q->start = new_start;

    return element;
}

int is_empty(Queue *q){

    return (q->end == NULL && q->start == NULL);
}

List *find_previous_element(List *element, Queue *q){
    
    List *itr = q->end;

    while(itr != NULL && itr->next != element){
        itr = itr->next;
    }

    return itr;
}

void  move_to_start(List *element, Queue *q){

	if (element == q->start) {
		return;
	}

    List *previous;
    previous = find_previous_element(element, q);

    if (previous != NULL){
        
        previous->next = element->next;
            
    } else {

		q->end = element->next;

	}

    element->next = NULL;

    enqueue_start(element, q);
}

void  delete_from_queue(List *element, Queue *q){

    List *previous;
    previous = find_previous_element(element, q);

    if (previous != NULL){
        
        previous->next = element->next;
            
    }else{

        q->end = element->next;
    
    }

    if(element->next == NULL){
        q->start = previous;
    }

    element->next = NULL;

    free_element(element);

}

void free_element(List *element){

    Plane *p = element->plane;

    element->plane = NULL;
    element->next = NULL;

    free_plane(p);
    
    free(element);

}

void free_queue(Queue *q){

    List *iterator = q->end;

    while(iterator != NULL){

        List *current = iterator;
        iterator = iterator->next;

        free_element(current);
    }

    q->start = NULL;
    q->end = NULL;

    free(q);
}