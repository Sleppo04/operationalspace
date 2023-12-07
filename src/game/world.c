#include "world.h"

void World_Create(world_t* world, unsigned int width, unsigned int height)
{
    ArrayListNew(&world->objects);
    world->sectors = NULL;
    world->sector_cols = width;
    world->sector_rows = height;

    return;
}

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

void World_DebugDump(world_t* world, char* filename)
{
    FILE* f;
    tile_t* t;

    f = fopen(filename, "wb");

    for (int x = 0; x < world->sector_cols * SECTOR_SIZE; x++) {
        for (int y = 0; y < world->sector_rows * SECTOR_SIZE; y++) {
            t = World_GetTile(world, x, y);
            fwrite(&t->glyph, 1, 1, f);
        }
    }

    fclose(f);
    printf("DEBUG: World (%ix%i) successfully dumped to %s!\n", world->sector_cols * SECTOR_SIZE, world->sector_rows * SECTOR_SIZE, filename);
    return;
}
