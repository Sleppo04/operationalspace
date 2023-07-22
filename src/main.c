#include <stdio.h>

//#include "squirrel.h"
#include "ui/window.h"
#include "ui/ui.h"
#include "game/sector.h"

int main(int argc, char** argv)
{
    window_t win;
    sector_t sector;

    Window_CreateWindow(80, 24, &win);
    Window_ClearScreen();
    Window_SetPos(1, 1);
    printf(":3 Hewwo Wowld uwu\n");
    UI_DrawSector(1, 1, &sector);
    UI_DrawFrame();
    Window_SetColor(15, 0);
    return 0;
}
