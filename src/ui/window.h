#ifndef WINDOW_H
#define WINDOW_H

#include "gfx.h"

typedef struct
{
    void* nativeHandle;
} window_t;

int  Window_CreateWindow(int width, int height, window_t* win);
void Window_DestroyWindow(window_t* win);
void Window_SetColor(color_t fg, color_t bg);
void Window_SetPos(int x, int y);
void Window_SetGlyph(int x, int y, glyph_t glyph);
void Window_PrintGlyph(glyph_t glyph);

#endif