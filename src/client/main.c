#include <stdio.h>

#include "ui/window.h"
#include "ui/ui.h"

int main()
{
    window_t win;
    
    Window_CreateWindow(80, 24, &win);
    UI_Init();
    Window_ClearScreen();
    UI_DrawFrame();
	
    getchar();
    Window_ResetColor();
	Window_DestroyWindow(&win);
    
	return 0;
}
