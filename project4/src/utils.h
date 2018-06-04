#ifndef UTILS_H
#define UTILS_H

#include "plane.h"
#include "queue.h"
#include "airport.h"

extern const int initial_time[2];
extern const int timestep_size;

void decrease_all_fuel(Queue *q);
char *get_time_from_start(int timesteps);

int has_emergency(Queue *q);

void plane_fall(List *element, Queue *q);

void land_plane(Airport *a, Plane *p, int lane, int global_time);

void takeoff_plane(Airport *a, Plane *p, int lane, int global_time);

void print_queue(Queue *q);

#endif /* UTILS_H */