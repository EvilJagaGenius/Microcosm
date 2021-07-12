#ifndef MCOSM_TILE_H_INCLUDED
#define MCOSM_TILE_H_INCLUDED

#include "mcosm_unit.h"
#include "mcosm_terrain.h"
#include "mcosm_feature.h"

typedef struct {
    mcosm_unit* unit;  // Can be nullptr, units will need to use dynamic memory
    mcosm_terrain terrain;
    mcosm_feature feature;
    int owner;
    int height;
} mcosm_tile;

#endif // MCOSM_TILE_H_INCLUDED
