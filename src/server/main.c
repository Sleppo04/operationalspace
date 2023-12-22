#include <stdio.h>

#include <world.h>
#include <sector.h>
#include <worldgen.h>

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
    feature_t features[2];
    xoshiro256_state_t state = {.state = {1, 2, 3, 4}};
	
    features[0].provider = feature_provider;
    features[0].maximum_noise_levels[0] = 250;
    features[0].minimum_noise_levels[0] = 215;
    features[1].provider                = NULL;

    World_Create(&world, 8, 8);
    WorldGen_GenerateWorld(&world, features, state);
	
	World_DebugDump(&world, "worldout");

	return 0;
}
