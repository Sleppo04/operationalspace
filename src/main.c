#include <stdio.h>

//#include "squirrel.h"
#include "ui/window.h"
#include "ui/ui.h"
#include "game/sector.h"

int main(int argc, char** argv)
{
    window_t win;
    world_t world;

    Window_CreateWindow(80, 24, &win);
    Window_ClearScreen();
    UI_DrawFrame();
    //UI_DrawWorld(0, 0, 16, 16, &world);
    Window_SetColor(15, 0);
    return 0;
}
