#include "world.h"

#define NEAREST_ARRAY_LENGTH 10

int get_place_feature_index(coordinate_t** nearest_features, feature_t* features, size_t feature_count, coordinate_t absolute_coordinate, struct drand48_data* rng_buffer, size_t* destination)
{
    if (nearest_features == NULL) {
        return EINVAL;
    }
    if (destination == NULL) {
        return ENOMEM;
    }

    coordinate_t* nearest_coordinates = malloc(sizeof(coordinate_t) * feature_count);
    if (nearest_coordinates == NULL) {
        return ENOMEM;
    }

    for (size_t i = 0; i < feature_count; i++) {
        nearest_coordinates[i].x = SIZE_MAX;
        nearest_coordinates[i].y = SIZE_MAX;
    }

    coordinate_t* best_coordinate;
    for (size_t feature_index = 0; feature_index < feature_count; feature_index++) {
        best_coordinate = nearest_coordinates + feature_index;
        for (size_t i = 0; i < NEAREST_ARRAY_LENGTH; i++) {
            coordinate_t* nearest_feature_coordinate = nearest_features[feature_index] + i;
            size_t self_distance;
            size_t best_distance;
            coordinate_manhattan_distance(best_coordinate, &absolute_coordinate, &best_distance);
            coordinate_manhattan_distance(nearest_feature_coordinate, &absolute_coordinate, &self_distance);

            if (self_distance < best_distance) {
            	best_coordinate[0] = nearest_feature_coordinate[0];
            }
        }
    }

    double best_probability = DBL_MAX;
    size_t best_index    = feature_count;
    feature_t* feature;
    size_t self_distance, foreign_distance;
    double feature_probability;
    double random_number;
    int random_code;
    for (size_t feature_index = 0; feature_index < feature_count; feature_index++) {
    	feature = features + feature_index;

    	coordinate_manhattan_distance(nearest_coordinates + feature_index, &absolute_coordinate, &self_distance);
    	if (self_distance < feature->minimum_distance) {
    		continue;
    	}

    	foreign_distance = SIZE_MAX;
    	coordinate_t* foreign_coordinate;
    	size_t coord_distance;
    	for (size_t f = 0; f < feature_count; f++) {
    		if (f == feature_index) continue;

    		foreign_coordinate = nearest_coordinates + f;
    		coordinate_manhattan_distance(foreign_coordinate, &absolute_coordinate, &coord_distance);

    		foreign_distance = min(foreign_distance, coord_distance);
    	}

    	if (foreign_distance < feature->foreign_distance) {
    		continue;
    	}


    	feature_probability  = feature->base_probability;
    	feature_probability += feature->probability_mod * self_distance;
    	feature_probability *= feature->probablity_growth * self_distance;

    	feature_probability  = max(feature->min_probability, feature_probability);
    	feature_probability  = min(feature->max_probability, feature_probability);

    	errno = 0;
    	random_code = drand48_r(rng_buffer, &random_number);
    	if (random_code < 0) {
    		free(nearest_coordinates);
    		random_code = errno;
    		perror("The following error occured in get_place_feature_index from world.c when calling drand48_r");
    		return random_code;
    	}
    	feature_probability -= random_number;

    	if (feature_probability < best_probability) {
    		best_probability = feature_probability;
    		best_index       = feature_index;
    	}
    }

    destination[0] = best_index;
    free(nearest_coordinates);

    return EXIT_SUCCESS;
}

int place_feature(feature_t* feature, tile_t* tile, coordinate_t coordinate, arraylist_t* feature_placement_list)
{
	if (feature == NULL) {
		return EINVAL;
	}
	if (tile == NULL) {
		return EDESTADDRREQ;
	}
	if (feature_placement_list == NULL) {
		return EINVAL;
	}

	int provider_code = feature->provider(&(tile->object), feature->user_data);
	if (provider_code) {
		fprintf(stderr, "Feature->provider returned %i in place_feature, exiting function\n", provider_code);
		return provider_code;
	}

	coordinate_t* m_coordinate = malloc(sizeof(coordinate_t));
	if (m_coordinate == NULL) {
		return ENOMEM;
	}

	int append_code = arrayListAppend(feature_placement_list, m_coordinate);
	if (append_code) {
		fprintf(stderr, "arrayListAppend returned %i in place_feature, exiting function\n", append_code);
		free(m_coordinate);
		return append_code;
	}

	return EXIT_SUCCESS;
}

int populate_sector(sector_t* sector, size_t row, size_t col, feature_t* features, size_t feature_count, arraylist_t* placed_features, struct drand48_data* rng_buffer)
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

    coordinate_t sector_mid = {.x = col * SECTOR_SIZE + SECTOR_SIZE / 2,
                               .y = row * SECTOR_SIZE + SECTOR_SIZE / 2};

    coordinate_t nearest_features[feature_count][NEAREST_ARRAY_LENGTH]; // Coordinates of the ten nearest features for each feature, measured from the middle of the sector
    
    for (size_t feature_index = 0; feature_index < feature_count; feature_index++) {
        arraylist_t*  feature_arraylist = placed_features + feature_index;
        coordinate_t* nearest_array = nearest_features[feature_index];
        coordinate_find_nearest(nearest_array, NEAREST_ARRAY_LENGTH, feature_arraylist, sector_mid);
    }


    size_t place_feature_index;
    coordinate_t sector_start;
    sector_start.x = col * SECTOR_SIZE;
    sector_start.y = row * SECTOR_SIZE;
    int rc;
    for (size_t coordinate_row = 0; coordinate_row < SECTOR_SIZE; coordinate_row++) {
        for (size_t coordinate_col = 0; coordinate_col < SECTOR_SIZE; coordinate_col++) {
            coordinate_t absolute_coordinate = {.x = sector_start.x + coordinate_col, .y = sector_start.y + coordinate_row};
            rc = get_place_feature_index((coordinate_t**) nearest_features, features, feature_count, absolute_coordinate, rng_buffer, &place_feature_index);

            if (rc) {
            	return rc;
            }

            if (place_feature_index != feature_count) {
            	tile_t* tile = &(sector->tiles[coordinate_row][coordinate_col]);
            	int place_code = place_feature(features + place_feature_index, tile, absolute_coordinate, placed_features + place_feature_index);
            	if (place_code) {
            		return place_code;
            	}
            }
        }
    }

    return EXIT_SUCCESS;
}

int generate_world(size_t sector_rows, size_t sector_cols, feature_t* features, world_t* destination, unsigned short seed[3])
{
    int seed_code;
    int populate_code;
    world_t local_world;
    size_t sector_bytes;
    size_t feature_count;
    sector_t* sector_array;
    arraylist_t* placed_features;
    struct drand48_data rng_buffer;

    
    if (sector_rows == 0)
        return EINVAL;
    if (sector_cols == 0)
        return EINVAL;
    if (features == NULL) 
        return EINVAL;
    if (destination == NULL) 
        return EDESTADDRREQ;
    
    seed_code = seed48_r(seed, &rng_buffer);
    if (seed_code < 0) {
    	fprintf(stderr, "Seeding the rng buffer failed in generate_world, exiting function");
    	return ECANCELED;
    }

    sector_bytes = sizeof(sector_t) * sector_rows * sector_cols;
    sector_array = (sector_t*) calloc(1, sector_bytes);
    if (sector_array == NULL)
        return ENOMEM;

    for (feature_count = 0; features[feature_count].provider != NULL; feature_count++);

    placed_features = malloc(sizeof(arraylist_t) * feature_count);
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
            World_GetSector(&local_world, row, col, &current_sector);
            populate_code = populate_sector(current_sector, row, col, features, feature_count, placed_features, &rng_buffer);
            if (populate_code) {
            	return populate_code;
            }
        }
    }

    return EXIT_SUCCESS;
}

int World_GetSector(world_t* world, size_t row, size_t col, sector_t** destination)
{
    size_t index;
    
    if (world == NULL)
        return EINVAL;
    if (row >= world->sector_rows)
        return EINVAL;
    if (col >= world->sector_cols)
        return EINVAL;
    if (destination == NULL)
        return EDESTADDRREQ;

    index = world->sector_rows * row + col;
    destination[0] = world->sectors + index;

    return EXIT_SUCCESS;
}

tile_t* World_GetTile(world_t* world, int x, int y)
{
    int sectorX;
    int sectorY;
    sector_t* sector;

    sectorX = floor((float)x / SECTOR_SIZE);
    sectorY = floor((float)y / SECTOR_SIZE);

    World_GetSector(world, sectorX, sectorY, &sector);

    return &sector->tiles[x%SECTOR_SIZE][y%SECTOR_SIZE];
}