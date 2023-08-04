#include <stdio.h>

#include "ui/window.h"
#include "ui/ui.h"
#include "game/sector.h"
#include "game/worldgen.h"

int feature_provider(gameobject_t** destination, void* user_data __attribute__((unused)))
{
	destination[0] = malloc(sizeof(gameobject_t));
	destination[0]->color = 100;
	destination[0]->glyph = '#';


	return 0;
}

int main()
{
    window_t win;
    world_t world;
    feature_t features[2];
    xoshiro256_state_t state = {.state = {1, 2, 3, 4}};
	
    features[0].provider = feature_provider;
    features[0].maximum_noise_levels[0] = 250;
    features[0].minimum_noise_levels[0] = 100;
    features[1].provider            = NULL;

    WorldGen_GenerateWorld(8, 8, features, &world, state);

    Window_CreateWindow(80, 24, &win);
    UI_Init(&world);
    Window_ClearScreen();
    UI_DrawFrame();
	
    Window_ResetColor();
	Window_DestroyWindow(&win);
    getchar();
    return 0;
}
