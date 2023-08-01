#include <stdio.h>

//#include "squirrel.h"
#include "ui/window.h"
#include "ui/ui.h"
#include "game/sector.h"

int feature_provider(gameobject_t** destination, void* user_data __attribute__((unused)))
{
	destination[0] = malloc(sizeof(gameobject_t));
	destination[0]->color = 100;
	destination[0]->glyph = '#';

	return 0;
}

int main(int argc, char** argv)
{
    window_t win;
    world_t world;
	feature_t features[2];
	
    features[0].provider = feature_provider;
	features[0].base_probability    = 0.001;
	features[0].foreign_distance    = 5;
	features[0].minimum_distance    = 5;
	features[0].min_probability     = 0.001;
	features[0].max_probability     = 0.01;
	features[0].probability_mod     = 0.0;
	features[0].probability_growth  = 1;
	features[1].provider            = NULL;

    generate_world(8, 8, features, &world, 42);

    Window_CreateWindow(80, 24, &win);
	UI_Init(&world);
    Window_ClearScreen();
    UI_DrawFrame();
	
    Window_ResetColor();
	Window_DestroyWindow(&win);
    getchar();
    return 0;
}
