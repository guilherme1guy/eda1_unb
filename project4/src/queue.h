#ifndef QUEUE_H
#define QUEUE_H

#include "plane.h"

typedef struct element {
    Plane *plane;
    struct element *next;
} List;


typedef struct queue {
    List *start;
    List *end;
} Queue;

List *create_element(Plane *p);
Queue *create_queue();

void enqueue(List *element, Queue *q);
void enqueue_start(List *element, Queue *q);
void move_to_start(List *element, Queue *q);

List *find_previous_element(List *element, Queue *q);
void delete_from_queue(List *element, Queue *q);

List *dequeue(Queue *q);

void free_element(List *element);
void free_queue(Queue *q);

int is_empty(Queue *q);

#endif /* QUEUE_H */