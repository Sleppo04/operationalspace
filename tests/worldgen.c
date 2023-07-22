#include "../src/game/world.c"
#include "../src/logic/arraylist/arraylist.c"
#include "../src/logic/coordinate/coordinate.c"

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
	unsigned int seed = 15;
	feature_t features[2];
	features[0].provider = feature_provider;
	features[0].base_probability    = 0.1;
	features[0].foreign_distance    = 5;
	features[0].minimum_distance    = 5;
	features[0].min_probability     = 0.001;
	features[0].max_probability     = 0.25;
	features[0].probability_mod     = 0.005;
	features[0].probability_growth  = 1;
	features[1].provider            = NULL;
	int world_code = generate_world(2, 2, features, &world, seed);

	printf("generate_world exited with %i\n",world_code);

	for (size_t row = 0; row < world.sector_rows * SECTOR_SIZE; row++) {
		for (size_t col = 0; col < world.sector_cols * SECTOR_SIZE; col++) {
			tile_t* tile = World_GetTile(&world, col, row);
			if (tile->object) free(tile->object);
		}
	}

	free(world.sectors);

	return 0;
}
