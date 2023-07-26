#ifndef UI_H
#define UI_H

#include <stdio.h>
#include "window.h"
#include "worldview.h"
#include "sidebar.h"
#include "../game/world.h"

void UI_Init(world_t* world);
void UI_DrawStatusBar();
void UI_DrawFrame();

#endif