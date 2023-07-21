#include "world.h"

int populate_sector(sector_t* sector, size_t row, size_t col, feature_t* features, size_t feature_count, arraylist_t* placed_features)
{
    if (sector == NULL) {
        return EDESTADDRREQ;
    }
    if (features == NULL) {
        return EINVAL;
    }
    if (placed_features == NULL) {
        return EINVAL;
    }
}

int generate_world(size_t sector_rows, size_t sector_cols, feature_t* features, world_t* destination)
{
    if (sector_rows == 0) {
        return EINVAL;
    }
    if (sector_cols == 0) {
        return EINVAL;
    }
    if (features == NULL) {
        return EINVAL;
    }
    if (destination == NULL) {
        return EDESTADDRREQ;
    }

    world_t local_world;

    size_t sector_bytes    = sizeof(sector_t) * sector_rows * sector_cols;
    sector_t* sector_array = (sector_t*) calloc(1, sector_bytes);
    if (sector_array == NULL) {
        return ENOMEM;
    }

    size_t feature_count;
    for (feature_count = 0; features[feature_count].provider != NULL; feature_count++);

    arraylist_t* placed_features = malloc(sizeof(arraylist_t) * feature_count);
    if (placed_features == NULL) {
        free(sector_array);
        return ENOMEM;
    }

    for (size_t i = 0; i < feature_count; i++) {
        int rc = ArrayListNew(placed_features + i);

        if (rc) {
            // hacky cleanup, lets go
            for (size_t fuck = 0; fuck < i; fuck++) {
                ArrayListDestroy(placed_features + fuck);
            }
            free(sector_array);
            free(placed_features);
            return ENOMEM;
        }
    }

    local_world.sectors = sector_array;
    local_world.sector_cols = sector_cols;
    local_world.sector_rows = sector_rows;

    for (size_t row = 0; row < sector_rows; row++) {
        for (size_t col = 0; col < sector_cols; col++) {
            sector_t* current_sector;
            world_get_sector(&local_world, row, col, &current_sector);
            populate_sector(current_sector, row, col, features, feature_count, placed_features);
        }
    }

    return EXIT_SUCCESS;
}

int world_get_sector(world_t* world, size_t row, size_t col, sector_t** destination)
{
    if (world == NULL) {
        return EINVAL;
    }
    if (row == 0 || row >= world->sector_rows) {
        return EINVAL;
    }
    if (col == 0 || col >= world->sector_cols) {
        return EINVAL;
    }
    if (destination == NULL) {
        return EDESTADDRREQ;
    }

    size_t index = world->sector_rows * row + col;
    destination[0] = world->sectors + index;

    return EXIT_SUCCESS;
}
