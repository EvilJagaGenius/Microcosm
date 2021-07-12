#ifndef MCOSM_UNIT_H_INCLUDED
#define MCOSM_UNIT_H_INCLUDED

#include "mcosm_unit_type.h"

typedef struct {  // A specific instance of a unit
    mcosm_unit_type unit_type;
    int owner;
    int veteran;
    int hp;
} mcosm_unit;

#endif // MCOSM_UNIT_H_INCLUDED
