#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <stdio.h> 

#include "../game/gameobject.h"
#include "../game/ship.h"
#include "../game/module.h"
#include "../game/tile.h"
#include "../ui/window.h"

typedef struct sidebar_s
{
    unsigned int screenX;
    unsigned int screenY;
    unsigned int width;
    unsigned int height;
} sidebar_t;

void UI_SidebarCreate(sidebar_t* sidebar, unsigned int screenX, unsigned int screenY, unsigned int width, unsigned int height);
void UI_SidebarRedraw(sidebar_t* sidebar, tile_t* tile);

#endif