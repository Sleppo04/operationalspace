#ifndef UI_H
#define UI_H

#include <stdio.h>
#include "window.h"
#include "../game/world.h"

void UI_DrawWorld(int worldX, int worldY, int width, int height, world_t world);
void UI_DrawFrame();

#endif