#include "worldgen.h"

int Worldgen_NewEmptyWorld(world_t* destination, size_t sector_rows, size_t sector_cols)
{
    destination->sector_cols = sector_cols;
    destination->sector_rows = sector_rows;

    size_t sector_bytes = sizeof(sector_t) * sector_cols * sector_rows;

    destination->sectors = malloc(sector_bytes);

    if (destination->sectors == NULL) {
        return ENOMEM;
    }

    return EXIT_SUCCESS;
}

bool WorldGen_CheckNoises(feature_t* feature, uint16_t* noise_values)
{
    for (int noise_index = 0; noise_index < NOISE_COUNT; noise_index++) {
        if (noise_values[noise_index] > feature->maximum_noise_levels[noise_index] || noise_values[noise_index] < feature->minimum_noise_levels[noise_index]) {
            return false;
        }
    }

    return true;
}

int WorldGen_PlaceFeature(tile_t* tile, feature_t* feature)
{
    return feature->provider(&(tile->object), feature->user_data);
}

int WorldGen_CheckFeaturePlacement(tile_t* tile, worldgendata_t* data, size_t x, size_t y)
{
    uint16_t* noise_values = malloc(sizeof(uint16_t) * NOISE_COUNT);
    if (noise_values == NULL) {
        return ENOMEM;
    }

    for (int noise_index = 0; noise_index < NOISE_COUNT; noise_index++) {
    	float result = OpenSimplex_2DNoise_ImprovedX(data->noise_seeds[noise_index], (double) x, (double) y);
        noise_values[noise_index] = (uint16_t) (fabs(result) * 1024);
    }

    int place_code;
    for (size_t feature_index = 0; feature_index < data->feature_count; feature_index++) {
        if (WorldGen_CheckNoises(&(data->features[feature_index]), noise_values)) {
            place_code = WorldGen_PlaceFeature(tile, &(data->features[feature_index]));
            if (place_code) {
                return place_code;
            }
            break;
        }
    }

    return EXIT_SUCCESS;
}

int WorldGen_GenerateWorld(uint16_t sector_rows, uint16_t sector_cols, feature_t* features, world_t* destination, xoshiro256_state_t rand_state)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    if (features == NULL) {
        return EINVAL;
    }
    if (sector_rows == 0 || sector_cols == 0) {
        return EINVAL;
    }

    worldgendata_t data;
    // maybe move this onto the heap
    data.local_world.seed = rand_state;

    data.features = features;
    for (data.feature_count = 0; features[data.feature_count].provider != NULL; data.feature_count++);

    for (int i = 0; i < NOISE_COUNT; i++) {
        data.noise_seeds[i] = xoshiro256_next(&(data.local_world.seed));
    }

    if (Worldgen_NewEmptyWorld(&(data.local_world), sector_rows, sector_cols)) {
        return ENOMEM;
    }

    size_t world_cols = sector_cols * SECTOR_SIZE;
    size_t world_rows = sector_rows * SECTOR_SIZE;
    int check_code;
    for (size_t row = 0; row < world_rows; row++) {
        for (size_t col = 0; col < world_cols; col++) {
            tile_t* tile = World_GetTile(&(data.local_world), row, col);
            check_code = WorldGen_CheckFeaturePlacement(tile, &data, col, row);
            if (check_code) {
                //TODO: Destroy world
                return check_code;
            }
        }
    }


    destination[0] = data.local_world;

    return EXIT_SUCCESS;
}
