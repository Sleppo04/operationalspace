#include "world.h"

int World_GetSector(world_t* world, size_t row, size_t col, sector_t** destination)
{
    size_t index;
    
    if (world == NULL)
        return EINVAL;
    if (row >= world->sector_rows)
        return EINVAL;
    if (col >= world->sector_cols)
        return EINVAL;
    if (destination == NULL)
        return EDESTADDRREQ;

    index = world->sector_rows * row + col;
    destination[0] = world->sectors + index;

    return EXIT_SUCCESS;
}

tile_t* World_GetTile(world_t* world, int x, int y)
{
    size_t sectorX;
    size_t sectorY;
    sector_t* sector;

    sectorX = floor((float)x / SECTOR_SIZE);
    sectorY = floor((float)y / SECTOR_SIZE);

    if(sectorX > (world->sector_cols - 1) || sectorY > (world->sector_rows - 1)) return NULL;

    World_GetSector(world, sectorX, sectorY, &sector);

    return &sector->tiles[x%SECTOR_SIZE][y%SECTOR_SIZE];
}
