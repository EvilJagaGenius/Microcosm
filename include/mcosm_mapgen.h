#ifndef MCOSM_MAPGEN_H_INCLUDED
#define MCOSM_MAPGEN_H_INCLUDED

#include <stdlib.h>
#include "mcosm_tile.h"
#include "mcosm_terrain.h"

mcosm_tile* WORLD;
int WORLD_X;
int WORLD_Y;

int randint(int lower, int upper);
void resizeMap();
void generateHeight();
void generateRiversHelper(int row, int col, int strength);
void generateRivers();
void generateWater();
void generateMap();

#endif // MCOSM_MAPGEN_H_INCLUDED
