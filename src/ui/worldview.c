#include "worldview.h"

void UI_WorldViewCreate(worldview_t* view, world_t* world, unsigned int screenX, unsigned int screenY, unsigned int width, unsigned int height)
{
    view->world   = world;
    view->worldX  = 0;
    view->worldY  = 0;
    view->cursorX = 0;
    view->cursorY = 0;
    view->screenX = screenX;
    view->screenY = screenY;
    view->width   = width;
    view->height  = height;

    return;
}

void UI_WorldViewRender(worldview_t* view)
{
    tile_t* tile;
    
    Window_SetPos(view->screenX, view->screenY);
    for (int i = 0; i < view->height; i++) {
        Window_SetPos(view->screenX, view->screenY + i);
        for (int j = 0; j < view->width; j++) {
            tile = World_GetTile(view->world, view->worldX + j, view->worldY + i);
            if (tile == NULL) {
                Window_SetColor(15, 1);
                Window_SetBlinking(true);
                Window_PrintGlyph('X');
                Window_SetBlinking(false);
            } else if (j == view->cursorX && i == view->cursorY) {
                // Draw selection cursor
                Window_SetColor(0, 3);
                Window_PrintGlyph(' ');
            } else if (tile->object != NULL) {
                Window_SetColor(tile->object->color, 0);
                Window_PrintGlyph(tile->object->glyph);
            } else {
                Window_SetColor(tile->color, 0);
                Window_PrintGlyph(tile->glyph);
            }
        }
    }
    
    return;
}