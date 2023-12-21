#include "ui.h"

void UI_Init()
{
    return;
}

void UI_DrawStatusBar()
{
    Window_SetPos(1, 24);
    printf("Cursor: %-4i, %-4i                                                              ", 0, 0);
    return;
}

void UI_DrawFrame()
{
    Window_SetColor(0, 7);
    // Side bar separator
    for (int i = 1; i < 24; i++) {
        Window_SetGlyph(60, i, ' ');
    }
    // Vertical separator
    Window_SetPos(1, 17);
    for (int i = 1; i < 60; i++) {
        Window_PrintGlyph(' ');
    }
    UI_DrawStatusBar();
    Window_SetPos(1, 17);
    printf("Script Editor");
    Window_SetColor(3, 0);
    Window_SetPos(1, 18);
    printf("Hiya~!\nI'm a script!\nYou can run me, if you reeeaaallyyy want to~! >_<\nUwU Rawr x3");
}
