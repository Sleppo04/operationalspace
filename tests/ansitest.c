#include <stdio.h>
#include <stdlib.h>
#include "../src/ui/window.h"

void RegisTest()
{
	Window_ClearScreen();
	Window_SetPos(1, 1);
	Window_SetColor(7, 0);
	Window_SetUnderlined(true);
	printf("Operational Space ReGIS Test\n\n");
	Window_ResetFormatting();
	printf("You should see a circle appear on screen.");
	printf("\x1bP0pP[%i,%i],F(C[+100])\x1b\\", 400, 240);
	return;
}

void SixelTest()
{
	Window_ClearScreen();
	Window_SetPos(1, 1);
	Window_SetColor(7, 0);
	Window_SetUnderlined(true);
	printf("Operational Space Sixel Test\n\n");
	Window_ResetFormatting();
	return;
}

int main()
{
    window_t win;
	char choice;
    
    Window_CreateWindow(80, 24, &win);
	Window_SetCursorVisible(false);
    Window_ClearScreen();
	Window_SetPos(1, 1);
	Window_SetColor(7, 0);
	Window_SetUnderlined(true);
	printf("OperationalSpace Terminal Capability Test\n\n");
	Window_ResetFormatting();

	
	// Text Formatting Test
	printf("Test Normal\n");
	Window_SetBold(true);
	printf("Test Bold...");
	Window_SetBold(false);
	printf(" no longer bold.\n");
	Window_SetItalic(true);
	printf("Test Italic...");
	Window_SetItalic(false);
	printf(" no longer italic.\n");
	Window_SetUnderlined(true);
	printf("Test Underlined...");
	Window_SetUnderlined(false);
	printf(" no longer underlined.\n");
	Window_SetBlinking(true);
	printf("Test Blinking...");
	Window_SetBlinking(false);
	printf(" no longer blinking.\n");
	Window_SetInverse(true);
	printf("Test Inverse...");
	Window_SetInverse(false);
	printf(" no longer inverse.\n");
	Window_SetCrossedout(true);
	printf("Test Crossedout...");
	Window_SetCrossedout(false);
	printf(" no longer crossedout.\n");

	// 16 Color Test
	printf("\n16 Colors:\n");
	// Foreground
	for (int i = 0; i < 16; i++) {
		Window_SetColor(i, 0);
		printf(" %02i ", i);
	}
	Window_PrintGlyph('\n');
	// Background
	for (int i = 0; i < 16; i++) {
		Window_SetColor(0, i);
		printf(" %02i ", i);
	}
	Window_SetColor(7, 0);

	// SetPos test (even though it's already been used...)
	Window_SetPos(1, 16);
	printf("CUP (Cursor Position) Test... ");
	Window_SetBold(true);
	Window_SetColor(15, 1);
	printf("FAILED!");
	Window_SetPos(31, 16);
	Window_SetColor(15, 2);
	printf("PASSED!");

	while (true) {
		Window_SetColor(7, 0);
		Window_SetPos(1, 23);
		printf("Hit ");
		Window_SetBlinking(true);
		Window_SetColor(1, 0);
		printf("[Q]");
		Window_SetColor(7, 0);
		Window_SetBlinking(false);
		printf(" to exit, ");
		Window_SetBlinking(true);
		Window_SetColor(3, 0);
		printf("[S]");
		Window_SetColor(7, 0);
		Window_SetBlinking(false);
		printf(" for SIXEL test and ");
		Window_SetBlinking(true);
		Window_SetColor(4, 0);
		printf("[R]");
		Window_SetColor(7, 0);
		Window_SetBlinking(false);
		printf(" for ReGIS test.");
		
		choice = getchar();
		switch (choice) {
			case 's':
				SixelTest();
				break;
			case 'r':
				RegisTest();
				break;
			case 'q':
				Window_ResetFormatting();
				Window_ResetColor();
			    Window_DestroyWindow(&win);
				exit(0);
				break;
		}
	}

	// Should never be reached
    return 1;
}