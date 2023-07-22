#include "../src/game/world.c"
#include "../src/logic/arraylist/arraylist.c"
#include "../src/logic/coordinate/coordinate.c"

#include <stdio.h>

int feature_provider(gameobject_t** destination, void* user_data)
{
	destination[0] = malloc(sizeof(gameobject_t));
	destination[0]->color = 100;
	destination[0]->glyph = '#';

	return 0;
}

int main()
{
	world_t world;
	feature_t features[2];
	unsigned short seed[3];
	seed[0] = 1;
	seed[1] = 2;
	seed[2] = 3;
	features[0].provider = feature_provider;
	features[0].base_probability = 0.1;
	features[0].foreign_distance = 5;
	features[0].minimum_distance = 5;
	features[0].min_probability  = 0.001;
	features[0].max_probability  = 0.75;
	features[0].probability_mod  = 0.05;
	features[0].probability_mod  = 1;
	features[1].provider         = NULL;
	int world_code = generate_world(2, 2, features, &world, seed);

	printf("generate_world exited with %i\n",world_code);

	return 0;
}
