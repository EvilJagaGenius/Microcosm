// Microcosm map generator.
#include "mcosm_mapgen.h"

int WORLD_X = 60;
int WORLD_Y = 40;
mcosm_tile* WORLD = NULL;

int WATER_LEVEL = 0;
int PRESSURE_POINT_ODDS = 50;
int MIN_PRESSURE = 2;
int MAX_PRESSURE = 8;
int POLAR_OCEAN_SIZE = 3;
int POLAR_DROPOFF = 2;
int POLAR_ODDS = 100;
int POLAR_HEIGHT = 50;
int MTN_THRESHOLD = 4;
int MTN_ODDS = 3;
int MTN_MOD = 2;
int RIVER_ODDS = 50;
int RIVER_MIN_STRENGTH = 3;
int RIVER_MAX_STRENGTH = 7;
int RIVER_BRANCH_ODDS = 50;

int randint(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

void resizeMap() {
    if (WORLD != NULL) {
        free(WORLD);
    }
    WORLD = malloc(sizeof(mcosm_tile) * WORLD_X * WORLD_Y);
    for (int row=0; row<WORLD_Y; row++) {
        for (int col=0; col<WORLD_X; col++) {
            WORLD[(row * WORLD_X) + col].unit = NULL;
            WORLD[(row * WORLD_X) + col].terrain = TERRAIN_TYPES[GRASSLAND];
            WORLD[(row * WORLD_X) + col].height = 0;
        }
    }
}

void generateHeight() {
    // The pressure-point map generator
    for (int row=0; row<WORLD_Y; row++) {
        for (int col=0; col<WORLD_X; col++) {
            int dieRoll = randint(1, PRESSURE_POINT_ODDS);
            if ((row + POLAR_OCEAN_SIZE >= WORLD_Y) || (row - POLAR_OCEAN_SIZE <= 0)) {  // If we're close to the poles
                dieRoll = randint(1, POLAR_ODDS);  // Use the polar odds instead
            }
            if (dieRoll == 1) {
                int pressure = randint(MIN_PRESSURE, MAX_PRESSURE);
                // Go in a simple radius around the pressure point
                for (int y=row-pressure; y<=row+pressure; y++) {
                    if ((y >= 0) && (y < WORLD_Y)) {
                        int width = pressure - abs(y - row);
                        for (int x=col-width; x<=col+width; x++) {
                            int prevHeight = WORLD[(y * WORLD_X) + (x % WORLD_X)].height;
                            WORLD[(y * WORLD_X) + (x % WORLD_X)].height += pressure - (abs(y-row) + abs(x-col));
                            if ((y + POLAR_OCEAN_SIZE >= WORLD_Y) || (y - POLAR_OCEAN_SIZE <= 0)) {
                                WORLD[(y * WORLD_X) + (x % WORLD_X)].height -= POLAR_DROPOFF;
                            }
                            if (prevHeight > MTN_THRESHOLD) {
                                int mtnDieRoll = randint(1, MTN_ODDS);
                                if (mtnDieRoll <= 1) {
                                    WORLD[(y * WORLD_X) + (x % WORLD_X)].terrain = TERRAIN_TYPES[MOUNTAINS];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // Finally, a ring of tiles around the north and south poles
    for (int col=0; col<WORLD_X; col++) {
        WORLD[col].height = POLAR_HEIGHT;
        WORLD[col].terrain = TERRAIN_TYPES[TUNDRA];
        WORLD[((WORLD_Y-1) * WORLD_X) + col].height = POLAR_HEIGHT;
        WORLD[((WORLD_Y-1) * WORLD_X) + col].terrain = TERRAIN_TYPES[TUNDRA];
    }
}

void generateRiversHelper(int row, int col, int strength) {
    // Recursive component of generateRivers()
    if (strength != 0) {
        mcosm_tile tile = WORLD[(row * WORLD_X) + col];
        WORLD[(row * WORLD_X) + col].terrain = TERRAIN_TYPES[RIVER];
        int nTile[2] = {row-1, col};
        int sTile[2] = {row+1, col};
        int eTile[2] = {row, (col+1) % WORLD_X};
        int wTile[2] = {row, (col-1) % WORLD_X};
        int* options[4] = {nTile, sTile, eTile, wTile};

        // Pick a direction at random, see if it works
        for (int i=0; i<4; i++) {
            int randomIndex = randint(0, 3);
            int* choiceLocation = options[randomIndex];
            mcosm_tile choice = WORLD[(choiceLocation[0] * WORLD_X) + choiceLocation[1]];
            if ((choice.terrain.symbol == 'g') &&
                (((strength < 0) && (choice.height <= tile.height)) ||
                 ((strength > 0) && (choice.height >= tile.height)))) {
                if ((strength < 0) && (choice.height <= tile.height)) {  // Negative strength: downhill
                    generateRiversHelper(choiceLocation[0], choiceLocation[1], strength+1);
                } else if ((strength > 0) && (choice.height >= tile.height)) {  // Positive strength: uphill
                    generateRiversHelper(choiceLocation[0], choiceLocation[1], strength-1);
                }

                // See if we branch
                int dieRoll = randint(1, RIVER_BRANCH_ODDS);
                if (dieRoll != 1)
                    break;
                //break;
            }
        }
    }
}

void generateRivers() {
    int sourceDirection = 0;  // Are we going uphill from water or downhill from a mountain?
    for (int row=0; row<WORLD_Y; row++) {
        for (int col=1; col<WORLD_X; col++) {
            // Get the tile and surrounding tiles
            mcosm_tile tile = WORLD[(row * WORLD_X) + col];
            mcosm_tile nTile = WORLD[((row-1) * WORLD_X) + col];
            mcosm_tile sTile = WORLD[((row+1) * WORLD_X) + col];
            mcosm_tile eTile = WORLD[(row * WORLD_X) + ((col+1) % WORLD_X)];
            mcosm_tile wTile = WORLD[(row * WORLD_X) + ((col-1) % WORLD_X)];
            mcosm_tile neighbors[] = {nTile, sTile, eTile, wTile};

            if (tile.terrain.symbol == 'g') {  // Only change grasslands, the default
                for (int i=0; i<4; i++) {
                    mcosm_tile surroundingTile = neighbors[i];
                    if ((surroundingTile.terrain.symbol == 'M') || (surroundingTile.height <= WATER_LEVEL)) {
                        if (surroundingTile.terrain.symbol == 'M') {
                            sourceDirection = -1;  // Go downhill
                        } else if (surroundingTile.height <= WATER_LEVEL) {
                            sourceDirection = 1;  // Go uphill
                        }
                        int dieRoll = randint(1, RIVER_ODDS);
                        if (dieRoll == 1) {  // Start a river
                            int strength = randint(RIVER_MIN_STRENGTH, RIVER_MAX_STRENGTH) * sourceDirection;
                            generateRiversHelper(row, col, strength);  // Recurse
                        }
                        break;  // Once we've found one qualifying tile, do not repeat for the other surrounding tiles
                    }
                }
            }
        }
    }
}

void generateTypes() {
    for (int row=0; row<WORLD_Y; row++) {
        for (int col=1; col<WORLD_X; col++) {
            mcosm_tile tile = WORLD[(row * WORLD_X) + col];
            mcosm_tile nTile = WORLD[((row-1) * WORLD_X) + col];
            mcosm_tile sTile = WORLD[((row+1) * WORLD_X) + col];
            mcosm_tile eTile = WORLD[(row * WORLD_X) + ((col+1) % WORLD_X)];
            mcosm_tile wTile = WORLD[(row * WORLD_X) + ((col-1) % WORLD_X)];
            mcosm_tile neighbors[] = {nTile, sTile, eTile, wTile};
            if (tile.terrain.symbol == 'g') {
                int tileTypes[] = {GRASSLAND, GRASSLAND, FOREST, SWAMP, HILLS, MOUNTAINS};
                int tileTypeOdds[] = {8, 20, 16, 8, 10, 2};
                if ((row < 10) || (row > WORLD_Y - 10)) {  // If we're close to the poles
                    tileTypes[0] = TUNDRA;
                } else if (abs(row - (WORLD_Y / 2)) < 5) {  // If we're close to the equator
                    tileTypes[0] = DESERT;
                }
                // Sum up the odds (replace with precalculated sum if needed)
                int sumOfOdds = 0;
                for (int i=0; i<6; i++)
                    sumOfOdds += tileTypeOdds[i];
                // Adjust odds based on surrounding tiles
                for (int i=0; i<4; i++) {
                    mcosm_tile surroundingTile = neighbors[i];
                    if (surroundingTile.terrain.symbol == TERRAIN_TYPES[tileTypes[0]].symbol) {  // Desert/Tundra
                        tileTypeOdds[1]--;
                        tileTypeOdds[0]++;
                    } else if (surroundingTile.terrain.symbol == 'f') {
                        tileTypeOdds[1]--;
                        tileTypeOdds[2]++;
                    } else if (surroundingTile.terrain.symbol == 's') {
                        tileTypeOdds[1]--;
                        tileTypeOdds[3]++;
                    } else if ((surroundingTile.terrain.symbol == 'm') || (surroundingTile.terrain.symbol == 'M')) {
                        tileTypeOdds[1]--;
                        tileTypeOdds[4]++;
                    } else if ((surroundingTile.terrain.symbol == 'r') || (surroundingTile.height <= WATER_LEVEL)) {
                        tileTypeOdds[0] -= 2;
                        tileTypeOdds[2]++;
                        tileTypeOdds[3]++;
                    }
                }
                int dieRoll = randint(1, sumOfOdds);
                int total = 0;
                // Based on that die roll, select the tile type
                for (int i=0; i<6; i++) {
                    if (dieRoll <= (total + tileTypeOdds[i])) {
                        WORLD[(row * WORLD_X) + col].terrain = TERRAIN_TYPES[tileTypes[i]];
                        break;
                    }
                    total += tileTypeOdds[i];
                }
            }
        }
    }
}

void generateWater() {
    for (int row=0; row<WORLD_Y; row++) {
        for (int col=0; col<WORLD_X; col++) {
            if (WORLD[(row * WORLD_X) + col].height <= WATER_LEVEL)
                WORLD[(row * WORLD_X) + col].terrain = TERRAIN_TYPES[WATER];
        }
    }
}

void generateMap() {
    resizeMap();
    generateHeight();
    generateRivers();
    generateTypes();
    //generateGoodies();
    generateWater();
}
