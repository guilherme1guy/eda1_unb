#ifndef PLANE_H
#define PLANE_H


typedef struct plane{
    int fuel;
    char type;
    char *name;
} Plane;

extern const char* PLANE_NAMES[];

void print_plane_name(Plane *p);
char *get_plane_name(Plane *p);
const char *get_random_plane_name();

void plane_action(Plane *p, int lane, int global_time);

int get_random_fuel();
char get_random_type();

void remove_fuel(Plane *p);

Plane *create_plane(const char *name, char type, int fuel);

void free_plane(Plane *p);

#endif /* PLANE_H */