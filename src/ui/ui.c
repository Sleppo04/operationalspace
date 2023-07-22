#include "ui.h"

void UI_DrawWorld(int worldX, int worldY, int width, int height, world_t world)
{
    tile_t* tile;
    
    Window_SetPos(0, 0);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            tile = World_GetTile(worldX + j, worldY + i);
            if (tile->object == NULL) {
                Window_SetColor(tile->object->color, 0);
                Window_PrintGlyph(tile->object->glyph);
            } else {
                Window_SetColor(tile->glyph, 0);
                Window_PrintGlyph(tile->glyph);
            }
        }
        Window_PrintGlyph('\n');
    }

    return;
}

void UI_DrawFrame()
{
    Window_SetColor(0, 7);
    // Side bar seperator
    for (int i = 1; i < 24; i++) {
        Window_SetGlyph(i, 60, ' ');
    }
    // Vertical separator
    Window_SetPos(17, 1);
    for (int i = 1; i < 60; i++) {
        Window_PrintGlyph(' ');
    }
    Window_SetPos(17, 1);
    printf("Script Editor");
    // Bottom status bar
    Window_SetPos(24, 1);
    for (int i = 1; i < 81; i++) {
        Window_PrintGlyph(' ');
    }
    Window_SetPos(24, 1);
    printf("Cursor: 123, 4 | Selection Mode | Selected Ship at: 120, 5");
    Window_SetColor(7, 0);
    Window_SetPos(1, 62);
    printf("Selected Ship");
    Window_SetPos(2, 62);
    printf("------------------");
    Window_SetPos(3, 62);
    printf("Name: MyShip");
    Window_SetPos(4, 62);
    printf("Script: ShipScript");
    Window_SetPos(6, 62);
    printf("Sp: 42");
    Window_SetPos(7, 62);
    printf("Hu: 42");
    Window_SetPos(8, 62);
    printf("Sh: 42");
    Window_SetPos(9, 62);
    printf("St: 42");
    Window_SetPos(10, 62);
    printf("Ba: 42");

    Window_SetColor(3, 0);
    Window_SetPos(18, 1);
    printf("Hiya~!\nI'm a script!\nYou can run me, if you reeeaaallyyy want to~! >_<\nUwU Rawr x3");
}