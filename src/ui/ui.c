#include "ui.h"

void UI_DrawSector(int x,int y, sector_t* sector)
{
    tile_t tile;

    Window_SetPos(x, y);
    for (int i = 0; i < SECTOR_SIZE; i++) {
        for(int j = 0; j < SECTOR_SIZE; j++) {
            tile = sector->tiles[j][i];
            Window_SetColor(0, 15);
            Window_PrintGlyph(' ');
        }
        Window_PrintGlyph('\n');
    }
    return;
}

void UI_DrawFrame()
{
    Window_SetColor(0, 7);
    for (int i = 1; i < 24; i++) {
        Window_SetGlyph(17, i, ' ');
    }
    Window_SetPos(1, 17);
    for (int i = 1; i < 80; i++) {
        Window_PrintGlyph('=');
    }
}