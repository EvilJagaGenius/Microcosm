#ifndef MCOSM_UNIT_TYPE_H_INCLUDED
#define MCOSM_UNIT_TYPE_H_INCLUDED

typedef struct {  // A definition of a type of unit
    int move;
    int attack;
    int defense;
    int maxHP;
    char symbol;
    int abilities;  // I think this will work like a set of binary flags... 0101 0101 0101 0101 where each 0 or 1 represents an ability that piece can or cannot do.  Use 16-bit ints?
    int costToBuild;
    // How do we represent unit prerequisites?
} mcosm_unit_type;

#endif // MCOSM_UNIT_TYPE_H_INCLUDED
