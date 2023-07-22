#include "coordinate.h"

int coordinate_manhattan_distance(coordinate_t *self, coordinate_t *other, size_t *destination)
{
    if (self == NULL) {
        return EINVAL;
    }
    if (other == NULL) {
        return EINVAL;
    }
    if (destination == NULL) {
        return EDESTADDRREQ;
    }

    size_t x_distance = max(self->x, other->x) - min(self->x, other->x);
    size_t y_distance = max(self->y, other->y) - min(self->y, other->y);

    destination[0] = x_distance + y_distance;

    return EXIT_SUCCESS;
}

int coordinate_find_nearest(coordinate_t *destination_array, size_t array_length, arraylist_t* source_list, coordinate_t position)
{
    if (destination_array == NULL) {
        return EDESTADDRREQ;
    }
    if (array_length == 0) {
        return EXIT_SUCCESS;
    }
    if (source_list == NULL) {
        return EINVAL;
    }

    size_t coordinates_found = 0;
    size_t insertion_position;
    size_t distance;
    for (size_t i = 0; i < source_list->size; i++) {
        coordinate_t* current;
        arrayListGet(source_list, i, (void**) &current);
        insertion_position = coordinates_found;
        coordinate_manhattan_distance(current, &position, &distance);
        

        for (size_t j = 0; j < coordinates_found; j++) {
            size_t other_distance;
            coordinate_t* other = destination_array + j;
            coordinate_manhattan_distance(other, &position, &other_distance);

            if (other_distance > distance) {
                insertion_position = j;
                break;
            }
        }

        if (insertion_position != array_length) {
            if (coordinates_found != array_length) {
                coordinates_found++;
            }
            coordinate_t* insertion_address = destination_array + insertion_position;
            size_t move_bytes = sizeof(coordinate_t) * (array_length - insertion_position - 1);
            memmove(insertion_address + 1, insertion_address, move_bytes);

            destination_array[insertion_position] = current[0];
        }
    }


    return EXIT_SUCCESS;
}
