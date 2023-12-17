#include "ui.h"

static worldview_t worldview;
static sidebar_t sidebar;

void UI_Init(world_t* world)
{
    UI_WorldViewCreate(&worldview, world, 1, 1, 59, 16);
    UI_SidebarCreate(&sidebar, 71, 1, 9, 24);
    return;
}

void UI_DrawStatusBar()
{
    Window_SetPos(1, 24);
    printf("Cursor: %-4i, %-4i                                                              ", worldview.cursorX, worldview.cursorY);
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
    UI_WorldViewRender(&worldview);
}