#include "mcosm_terrain.h"

const mcosm_terrain TERRAIN_TYPES[9] = {
    {  // Desert
        'd',
        6,
        4
    },
    {  // Tundra
        't',
        7,
        3
    },
    {  // Grassland
        'g',
        10,
        2
    },
    {  // Forest
        'f',
        2,
        0
    },
    {  // Swamp
        's',
        0,
        2
    },
    {  // Hills
        'm',
        6,
        2
    },
    {  // Mountains
        'M',
        8,
        7
    },
    {  // River
        'r',
        3,
        1
    },
    {  // Water
        'w',
        1,
        3
    }
};
