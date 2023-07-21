#ifndef UI_H
#define UI_H

#include "window.h"
#include "../game/sector.h"

void UI_DrawSector(int x,int y, sector_t* sector);
void UI_DrawFrame();

#endif