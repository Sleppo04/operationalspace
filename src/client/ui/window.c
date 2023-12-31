#include "window.h"

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

static struct termios termModeOld;

int Window_CreateWindow(int width, int height, window_t* win)
{
    // On Linux, we dont care about windows, we access the terminal.
    struct termios termMode;

    tcgetattr(STDIN_FILENO, &termMode);
    termModeOld = termMode;
    termMode.c_iflag &= ~(ICRNL | INLCR | IXON); // Disable automatic \r->\n (and vice versa, just in case), and flow control with ^Q and ^V
    termMode.c_lflag &= ~(ECHO | ICANON | ISIG); // Disable ^C,^Z, echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termMode);

    win->nativeHandle = (void*) 0x1; // Dummy value
    return 0;
}

void Window_DestroyWindow(window_t* win)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termModeOld);
    win->nativeHandle = NULL;
    return;
}

#elif defined _WIN32

#include <windows.h>

static HANDLE stdoutHandle;
static HANDLE stdinHandle;
static DWORD outModeOld;
static DWORD inModeOld;

int Window_CreateWindow(int width, int height, window_t* win)
{
    DWORD outMode;
    DWORD inMode;
    
    stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(stdoutHandle, &outMode);
    GetConsoleMode(stdinHandle, &inMode);
    outModeOld = outMode;
    inModeOld = inMode;
    
    // Activate ANSI stuff on Win10
    outMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(stdoutHandle, outMode);
    // Activate Raw Input Mode
    inMode = ENABLE_PROCESSED_INPUT | 0x0200 | ENABLE_EXTENDED_FLAGS; // ENABLE_VIRTUAL_TERMINAL_INPUT
    SetConsoleMode(stdinHandle, inMode);

    // TODO: Resize Console Window
    return 0;
}

void Window_DestroyWindow(window_t* win)
{
    // Reset console modes
    SetConsoleMode(stdoutHandle, outModeOld);
    SetConsoleMode(stdinHandle, inModeOld);
    return;
}

#else

#error "The Terminal Library doesn't support this OS."

#endif

void Window_SetColor(color_t fg, color_t bg)
{
    printf("\x1b[38;5;%um", fg);
    printf("\x1b[48;5;%um", bg);
    return;
}

void Window_SetPos(int x, int y)
{
    printf("\x1b[%u;%uH", y, x);
    return;
}

void Window_ResetColor()
{
    printf("\x1b[39m"); // Reset foreground color
    printf("\x1b[49m"); // Reset Background color
    return;
}

void Window_ResetFormatting()
{
    printf("\x1b[0m");
    return;
}

void Window_SetBold(bool a)
{
    if (a) printf("\x1b[1m");
    else printf("\x1b[22m");
    return;
}

void Window_SetItalic(bool a)
{
    if (a) printf("\x1b[3m");
    else printf("\x1b[23m");
    return;
}

void Window_SetUnderlined(bool a)
{
    if (a) printf("\x1b[4m");
    else printf("\x1b[24m");
    return;
}

void Window_SetBlinking(bool a)
{
    if (a) printf("\x1b[5m");
    else printf("\x1b[25m");
    return;
}

void Window_SetInverse(bool a)
{
    if (a) printf("\x1b[7m");
    else printf("\x1b[27m");
    return;
}

void Window_SetCrossedout(bool a)
{
    if (a) printf("\x1b[9m");
    else printf("\x1b[29m");
    return;
}

void Window_SetCursorVisible(bool a)
{
    if (a) printf("\x1b?25h");
    else printf("\x1b?25l");
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