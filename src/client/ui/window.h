#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <stdbool.h>
#include "gfx.h"

typedef struct
{
    void* nativeHandle;
} window_t;

int  Window_CreateWindow(int width, int height, window_t* win);
void Window_DestroyWindow(window_t* win);
void Window_SetColor(color_t fg, color_t bg);
void Window_ResetColor();
void Window_ResetFormatting();
void Window_SetBold(bool a);
void Window_SetItalic(bool a);
void Window_SetUnderlined(bool a);
void Window_SetBlinking(bool a);
void Window_SetInverse(bool a);
void Window_SetCrossedout(bool a);
void Window_SetCursorVisible(bool a);
void Window_SetPos(int x, int y);
void Window_SetGlyph(int x, int y, glyph_t glyph);
void Window_PrintGlyph(glyph_t glyph);
void Window_ClearScreen();

#endif