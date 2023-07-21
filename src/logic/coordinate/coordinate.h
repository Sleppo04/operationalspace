#ifndef COORDINATE_H
#define COORDINATE_H

#include <errno.h>
#include <stdlib.h>

#include "../arraylist/arraylist.h"

typedef struct Coordinate {
    size_t x;
    size_t y;
} coordinate_t;

int coordinate_manhattan_distance(coordinate_t* self, coordinate_t* other, size_t* destination);

int coordinate_find_nearest(coordinate_t* destination_array, size_t array_length, arraylist_t* source_list, coordinate_t position);

#endif // COORDINATE_H