#include "window.h"

#include <stdio.h>

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

    win->nativeHandle = 1; // Dummy value
    return 0;
}

void Window_DestroyWindow(window_t* win)
{
    // No need to destroy anything...
    return 0;
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
    printf("\x1b[%u;%uH");
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