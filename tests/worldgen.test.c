#include "world.h"
#include "worldgen.h"
#include "arraylist/arraylist.h"
#include "coordinate/coordinate.h"

#include <stdio.h>

int feature_provider(gameobject_t** destination, void* user_data __attribute__((unused)))
{
	destination[0] = malloc(sizeof(gameobject_t));
	destination[0]->color = 100;
	destination[0]->glyph = '#';

	return 0;
}

int main()
{
	world_t world;
	xoshiro256_state_t seed;
	seed.state[0] = 15;
	seed.state[1] = 15;
	seed.state[2] = 15;
	seed.state[3] = 15;

	feature_t features[2];
	features[0].provider = feature_provider;
	memset(features[0].minimum_noise_levels, 0x00, NOISE_COUNT * sizeof(uint16_t));
	memset(features[0].maximum_noise_levels, 0xFF, NOISE_COUNT * sizeof(uint16_t));
	features[1].provider             = NULL;
	int world_code = WorldGen_GenerateWorld(&world, features, seed);

	printf("generate_world exited with %i\n",world_code);


	for (size_t row = 0; row < world.sectorsY * SECTOR_SIZE; row++) {
		for (size_t col = 0; col < world.sectorsX * SECTOR_SIZE; col++) {
			tile_t* tile = World_GetTile(&world, col, row);
			if (tile->object) free(tile->object);
		}
	}

	free(world.sectors);

	return 0;
}
