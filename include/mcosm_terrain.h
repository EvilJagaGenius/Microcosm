#ifndef MCOSM_TERRAIN_H_INCLUDED
#define MCOSM_TERRAIN_H_INCLUDED

typedef struct {
    char symbol;
    int color1;
    int color2;
    // The variables below taken from the Test of Time terrain booklet
    /*int food;
    int production;
    int trade;
    int moveCost;
    float defBonus;
    int irrigationBonus;
    char irrigationBonusType;
    int miningBonus;
    char miningBonusType;
    int roadBonus;
    char roadBonusType;
    struct mcosm_terrain transformInto;  // Should this be a pointer?
    bool passable;*/
} mcosm_terrain;

enum TERRAIN_NAMES{DESERT, TUNDRA, GRASSLAND, FOREST, SWAMP, HILLS, MOUNTAINS, RIVER, WATER};
const mcosm_terrain TERRAIN_TYPES[9];


#endif // MCOSM_TERRAIN_H_INCLUDED
