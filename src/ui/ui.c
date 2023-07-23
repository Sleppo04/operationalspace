#include "ui.h"

void UI_DrawStatusBar()
{
    Window_SetPos(24, 1);
    printf("Cursor: %-4i, %-4i                                                              ", 123, 4);
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
    UI_DrawStatusBar();
    Window_SetPos(17, 1);
    printf("Script Editor");
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