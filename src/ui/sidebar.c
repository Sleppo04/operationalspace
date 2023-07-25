#include "sidebar.h"

void UI_SidebarCreate(sidebar_t* sidebar, unsigned int screenX, unsigned int screenY, unsigned int width, unsigned int height)
{
    sidebar->screenX = screenX;
    sidebar->screenY = screenY;
    sidebar->width = width;
    sidebar->height = height;
    return;
}

void UI_SidebarRedraw(sidebar_t* sidebar, tile_t* tile)
{
    int y;
    gameobject_t* obj;

    y = sidebar->screenY;
    Window_SetPos(sidebar->screenX + 1, sidebar->screenY);

    if (tile->object == NULL) {
        printf("Nothing here");
    } else {
        obj = tile->object;
        printf("Selected Ship");
        Window_SetPos(sidebar->screenX + 1, y);
        for (int i = 0; i < sidebar->width - 1; i++) Window_PrintGlyph('-'); // Draw Separator
        Window_SetPos(sidebar->screenX + 1, y);
        printf("Name: SHIPNAME");
        Window_SetPos(sidebar->screenX + 1, y);
        printf("Script: SCRIPTNAME");
        y++;
        // Display stats
        Window_SetPos(sidebar->screenX + 1, ++y);
        printf("Sp: %i", obj->stats.speed);
        Window_SetPos(sidebar->screenX + 1, ++y);
        printf("Hu: %i", obj->stats.hull);
        Window_SetPos(sidebar->screenX + 1, ++y);
        printf("Sh: %i", obj->stats.shield);
        Window_SetPos(sidebar->screenX + 1, ++y);
        printf("St: %i", obj->stats.storage);
        Window_SetPos(sidebar->screenX + 1, ++y);
        printf("Ba: %i", obj->stats.battery);
    }

    return;
}