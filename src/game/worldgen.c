#include "worldgen.h"

int get_place_feature_index(coordinate_t** nearby_feature_coordinates, feature_t* features, size_t feature_count, coordinate_t absolute_coordinate, unsigned int* seedp, size_t* destination)
{
    if (nearby_feature_coordinates == NULL) {
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
        for (size_t i = 0; i < NEARBY_ARRAY_LENGTH; i++) {
            coordinate_t* nearest_feature_coordinate = nearby_feature_coordinates[feature_index] + i;
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
    size_t best_index       = feature_count;
    feature_t* feature;
    size_t self_distance, foreign_distance;
    double feature_probability;
    double random_number;
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

        
        random_number = rand_r(seedp) / (double) (RAND_MAX);

        // Unused return because the conditions it checked are guaranteed in this function
        Feature_CalculateProbability(feature, self_distance, foreign_distance, &feature_probability);

    	feature_probability -= random_number;

    	if (feature_probability < best_probability && feature_probability > 0) {
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
	m_coordinate[0] = coordinate;

	int append_code = arrayListAppend(feature_placement_list, m_coordinate);
	if (append_code) {
		fprintf(stderr, "arrayListAppend returned %i in place_feature, exiting function\n", append_code);
		free(m_coordinate);
		return append_code;
	}

	return EXIT_SUCCESS;
}

int populate_sector(sector_t* sector, size_t row, size_t col, feature_t* features, size_t feature_count, arraylist_t* placed_features, unsigned int* seedp)
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

    coordinate_t** nearby_features = malloc(sizeof(coordinate_t*) * feature_count);// Coordinates of the ten nearest features for each feature, measured from the middle of the sector
    if (nearby_features == NULL) {
    	return ENOMEM;
    }
    coordinate_t* nearby_array_inner = malloc(sizeof(coordinate_t) * feature_count * NEARBY_ARRAY_LENGTH);
    if (nearby_array_inner == NULL) {
    	free(nearby_features);
    	return ENOMEM;
    }
    for (size_t i = 0; i < feature_count; i++) {
    	nearby_features[i] = nearby_array_inner + (i * NEARBY_ARRAY_LENGTH);
    }

    for (size_t feature_index = 0; feature_index < feature_count; feature_index++) {
    	for (size_t i = 0; i < NEARBY_ARRAY_LENGTH; i++) {
    		nearby_features[feature_index][i].x = SIZE_MAX;
    		nearby_features[feature_index][i].y = SIZE_MAX;
    	}
    }


    size_t place_feature_index;
    coordinate_t sector_start;
    sector_start.x = col * SECTOR_SIZE;
    sector_start.y = row * SECTOR_SIZE;
    int rc;
    for (size_t coordinate_row = 0; coordinate_row < SECTOR_SIZE; coordinate_row++) {
        for (size_t coordinate_col = 0; coordinate_col < SECTOR_SIZE; coordinate_col++) {
            for (size_t feature_index = 0; feature_index < feature_count; feature_index++) {
                arraylist_t*  feature_arraylist = placed_features + feature_index;
                coordinate_t* nearby_array = nearby_features[feature_index];
                coordinate_find_nearest(nearby_array, NEARBY_ARRAY_LENGTH, feature_arraylist, sector_mid);
            }

            coordinate_t absolute_coordinate = {.x = sector_start.x + coordinate_col, .y = sector_start.y + coordinate_row};
            rc = get_place_feature_index((coordinate_t**) nearby_features, features, feature_count, absolute_coordinate, seedp, &place_feature_index);

            if (rc) {
            	free(nearby_array_inner);
            	free(nearby_features);
            	return rc;
            }

            tile_t* tile = &(sector->tiles[coordinate_row][coordinate_col]);
            tile->color  = 15;
            tile->glyph  = ' ';

            if (place_feature_index != feature_count) {
            	int place_code = place_feature(features + place_feature_index, tile, absolute_coordinate, placed_features + place_feature_index);
            	if (place_code) {
                	free(nearby_array_inner);
                	free(nearby_features);
            		return place_code;
            	}
            }
        }
    }

	free(nearby_array_inner);
	free(nearby_features);
    return EXIT_SUCCESS;
}

int WorldGenData_Create(worldgendata_t* data, size_t sector_rows, size_t sector_cols, world_t* destination_world, feature_t* features, xoshiro256_state_t* rand_state)
{
    if (data == NULL) {
        return EDESTADDRREQ;
    }

    worldgendata_t local;
    local.destination        = destination_world;
    local.features           = features;
    for (local.feature_count = 0; features[local.feature_count].provider != NULL; local.feature_count++);
    local.feature_positions  = malloc(sizeof(dynamic_buffer_t) * local.feature_count);
    if (local.feature_positions == NULL) {
        return ENOMEM;
    }
    local.local_world.sector_cols = sector_cols;
    local.local_world.sector_rows = sector_rows;

    // Fill the array with buffers
    int buffer_code;
    for (size_t feature_index = 0; feature_index < local.feature_count; feature_index++) {
        dynamic_buffer_t* buffer_position = local.feature_positions + feature_index;
        buffer_code = DynamicBuffer_Create(buffer_position, sizeof(coordinate_t) * 32);

        if (buffer_code) {
            // ENOMEM
            for (size_t i = 0; i < feature_index; i++) {
                DynamicBuffer_Destroy(local.feature_positions + i);
            }
            free(local.feature_positions);

            return ENOMEM;
        }
    }

    return EXIT_SUCCESS;
}

int generate_world(size_t sector_rows, size_t sector_cols, feature_t* features, world_t* destination, xoshiro256_state_t* rand_state)
{
    int populate_code;
    size_t sector_bytes;
    size_t feature_count;
    worldgendata_t data;·
    
    if (sector_rows == 0)
        return EINVAL;
    if (sector_cols == 0)
        return EINVAL;
    if (features == NULL) 
        return EINVAL;
    if (destination == NULL) 
        return EDESTADDRREQ;
    

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

    local_world.sectors     = sector_array;
    local_world.sector_cols = sector_cols;
    local_world.sector_rows = sector_rows;
    local_world.seed        = seed;

    for (size_t row = 0; row < sector_rows; row++) {
        for (size_t col = 0; col < sector_cols; col++) {
            sector_t* current_sector;
            World_GetSector(&local_world, row, col, &current_sector);
            populate_code = populate_sector(current_sector, row, col, features, feature_count, placed_features, &(local_world.seed));
            if (populate_code) {
            	for (size_t fuck = 0; fuck < feature_count; fuck++) {
					arraylist_t* list = placed_features + fuck;
					for (size_t i = 0; i < list->size; i++) {
						free(list->array[i]);
					}
					ArrayListDestroy(list);
				}
            	free(placed_features);
            	free(sector_array);
            	return populate_code;
            }
        }
    }

	for (size_t fuck = 0; fuck < feature_count; fuck++) {
		arraylist_t* list = placed_features + fuck;
		for (size_t i = 0; i < list->size; i++) {
			free(list->array[i]);
		}
		ArrayListDestroy(list);
	}
    free(placed_features);

    destination[0] = local_world;

    return EXIT_SUCCESS;
}