#include "../src/logic/coordinate/coordinate.h"
#include "../src/logic/arraylist/arraylist.h"

#include <stdio.h>

int main()
{
    coordinate_t target_array[3];
    arraylist_t list;
    ArrayListNew(&list);
    coordinate_t coord1 = {.x = 5, .y = 3};
    coordinate_t coord2 = {.x = 1, .y = 2};
    coordinate_t coord3 = {.x = 8, .y = 2};
    coordinate_t coord4 = {.x = 10, .y = 3};
    coordinate_t coord5 = {.x = 2, .y = 5};
    arrayListAppend(&list, &coord1);
    arrayListAppend(&list, &coord2);
    arrayListAppend(&list, &coord3);
    arrayListAppend(&list, &coord4);
    arrayListAppend(&list, &coord5);

    coordinate_t source = {.x = 1, .y = 1};

    coordinate_find_nearest(target_array, 3, &list, source);

    printf("%lu|%lu\n", target_array[0].x, target_array[0].y);
    printf("%lu|%lu\n", target_array[1].x, target_array[1].y);
    printf("%lu|%lu\n", target_array[2].x, target_array[2].y);


    ArrayListDestroy(&list);

    return 0;
}