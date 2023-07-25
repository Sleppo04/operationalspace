#include "ui.h"

void UI_DrawStatusBar()
{
    Window_SetPos(1, 24);
    printf("Cursor: %-4i, %-4i                                                              ", 123, 4);
}

void UI_DrawFrame()
{
    Window_SetColor(0, 7);
    // Side bar seperator
    for (int i = 1; i < 24; i++) {
        Window_SetGlyph(60, i, ' ');
    }
    // Vertical separator
    Window_SetPos(17, 1);
    for (int i = 1; i < 60; i++) {
        Window_PrintGlyph(' ');
    }
    UI_DrawStatusBar();
    Window_SetPos(1, 17);
    printf("Script Editor");
    Window_SetColor(7, 0);
    Window_SetPos(62, 1);
    printf("Selected Ship");
    Window_SetPos(62, 2);
    printf("------------------");
    Window_SetPos(62, 3);
    printf("Name: MyShip");
    Window_SetPos(62, 4);
    printf("Script: ShipScript");
    Window_SetPos(62, 6);
    printf("Sp: 42");
    Window_SetPos(62, 7);
    printf("Hu: 42");
    Window_SetPos(62, 8);
    printf("Sh: 42");
    Window_SetPos(62, 9);
    printf("St: 42");
    Window_SetPos(62, 10);
    printf("Ba: 42");

    Window_SetColor(3, 0);
    Window_SetPos(1, 18);
    printf("Hiya~!\nI'm a script!\nYou can run me, if you reeeaaallyyy want to~! >_<\nUwU Rawr x3");
}