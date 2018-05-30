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
    }

    q->start = NULL;
    q->end = NULL;

    return q;
}

Queue *enqueue(List *element, Queue *q){
    
    if (is_empty(q)) {
        
        q->start = element;
        q->end = element;
    
    }else{
        
        element->next = q->end;
        q->end = element;

    }

    return q;
}

Queue *enqueue_start(List *element, Queue *q){
    
    if (is_empty(q)) {
        
        q->start = element;
        q->end = element;
    
    }else{
        
        q->start->next = element;
        q->start = element;

    }

    return q;
}

List *dequeue(Queue *q){

    if (is_empty(q)){

        return NULL;

    }

    List *element = q->start;

    List *new_start = q->end;

    if(new_start->next != NULL){

        while(new_start->next != element){
            new_start = new_start->next;
        }
    
    }
    
    new_start->next = NULL;
    q->start = new_start;

    return element;
}

int is_empty(Queue *q){

    return (q->end == NULL || q->start == NULL);
}

List *find_previous_element(List *element, Queue *q){
    
    List *previous = NULL;

    List *iterator = q->end;
    while (iterator != NULL && iterator->next != element){
        iterator = iterator->next;
    }

    previous = iterator;

    return previous;
}

Queue *move_to_start(List *element, Queue *q){

    List *previous;
    previous = find_previous_element(element, q);

    if (previous != NULL){
        
        previous->next = element->next;
            
    }

    element->next = NULL;

    return enqueue_start(element, q);
}

Queue *delete_from_queue(List *element, Queue *q){

    List *previous;
    previous = find_previous_element(element, q);

    if (previous != NULL){
        
        previous->next = element->next;
            
    }else{

        q->end = element->next;
    
    }

    element->next = NULL;

    free_element(element);

    return q;
}

void free_element(List *element){

    free_plane(element->plane);
    
    free(element);
}

void free_queue(Queue *q){

    List *iterator = q->end;

    while(iterator->next != NULL){

        List *current = iterator;
        iterator = iterator->next;

        free_element(current);
    }

    free(q);
}