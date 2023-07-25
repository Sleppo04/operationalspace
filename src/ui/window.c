#include "window.h"

#ifdef __linux__

#include <sys/ioctl.h>

int Window_CreateWindow(int width, int height, window_t* win)
{
    // On Linux, we dont care about windows, we access the terminal.
    // Set terminal to requested size, no matter its current size
    struct winsize ws;
    ws.ws_col = width;
    ws.ws_row = height;
    if (ioctl(0, TIOCSWINSZ, &ws)) {
        printf("ERROR: Couldn't set terminal size!\n");
        return 1;
    }

    win->nativeHandle = (void*) 0x1; // Dummy value
    return 0;
}

void Window_DestroyWindow(window_t* win)
{
    // No need to destroy anything...
    win->nativeHandle = NULL;
    return;
}

#elif defined _WIN32

#include <windows.h>

int Window_CreateWindow(int width, int height, window_t* win)
{
    DWORD outMode;
    HANDLE stdoutHandle;
    
    // Activate ANSI Stuff on Win10
    stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(stdoutHandle, &outMode);
    outMode |= 0x0004; //ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(stdoutHandle, outMode);

    // TODO: Resize Console Window
    return 0;
}

void Window_DestroyWindow(window_t* win)
{
    // No need to destroy anything...
    return;
}

#endif

void Window_SetColor(color_t fg, color_t bg)
{
    printf("\x1b[38;5;%um", fg);
    printf("\x1b[48;5;%um", bg);
    return;
}

void Window_SetPos(int x, int y)
{
    printf("\x1b[%u;%uH", x, y);
    return;
}

void Window_ResetColor()
{
    printf("\x1b[39m"); // Reset foreground color
    printf("\x1b[49m"); // Reset Background color
}

void Window_SetBold(bool a)
{
    if (a) printf("\x1b[1m");
    else printf("\x1b[22");
    return;
}

void Window_SetItalic(bool a)
{
    if (a) printf("\x1b[3m");
    else printf("\x1b[23");
    return;
}

void Window_SetUnderlined(bool a)
{
    if (a) printf("\x1b[4m");
    else printf("\x1b[24");
    return;
}

void Window_SetBlinking(bool a)
{
    if (a) printf("\x1b[5m");
    else printf("\x1b[25");
    return;
}

void Window_SetInverse(bool a)
{
    if (a) printf("\x1b[7m");
    else printf("\x1b[27");
    return;
}

void Window_SetCrossedout(bool a)
{
    if (a) printf("\x1b[9m");
    else printf("\x1b[29");
    return;
}

void Window_SetGlyph(int x, int y, glyph_t glyph)
{
    Window_SetPos(x, y);
    Window_PrintGlyph(glyph);
    return;
}

void Window_PrintGlyph(glyph_t glyph)
{
    putchar(glyph);
    return;
}

void Window_ClearScreen()
{
    printf("\x1b[2J");
    return;
}