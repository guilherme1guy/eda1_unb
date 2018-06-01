#ifndef AIRPORT_H
#define AIRPORT_H

#include "plane.h"
#include "queue.h"

typedef struct lane{
    int id;
    int special;
    int in_use_for_time;
} Lane;

typedef struct airport{
    Lane **lanes;
    int count;
} Airport;

extern const int approach_time;
extern const int land_time;
extern const int take_off_time;

Airport *start_airport();

void update_airport_lanes(Airport *a);

void free_airport(Airport *a);

int can_land(Airport *a, int lane, int emergency_status);
int can_takeoff(Airport *a, int lane, int emergency_status);

#endif /* AIRPORT_H */