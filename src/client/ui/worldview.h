#ifndef WORLDVIEW_H
#define WORLDVIEW_H

#include "../game/world.h"
#include "window.h"
#include "gfx.h"

typedef struct worldview_s
{
    world_t*     world;
    unsigned int worldX;
    unsigned int worldY;
    unsigned int cursorX;
    unsigned int cursorY;
    unsigned int screenX;
    unsigned int screenY;
    unsigned int width;
    unsigned int height;
} worldview_t;

void UI_WorldViewCreate(worldview_t* view, world_t* world, unsigned int screenX, unsigned int screenY, unsigned int width, unsigned int height);
void UI_WorldViewRender(worldview_t* view);

#endif