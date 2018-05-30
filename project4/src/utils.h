#ifndef UTILS_H
#define UTILS_H

#include "plane.h"
#include "queue.h"

void decrease_all_fuel(Queue *q);
char *get_time_from_start(int *initial_time, int timesteps, int timestep_size);

void plane_fall(List *element, Queue *q);

#endif /* UTILS_H */