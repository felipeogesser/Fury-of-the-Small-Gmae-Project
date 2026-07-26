#ifndef GENERAL_INTERNAL_H
#define GENERAL_INTERNAL_H

#include "general_types.h"
#include "battalion_types.h"
#include "sprites_types.h"

typedef struct General {
    unsigned int id;
    signed int hp;
    signed int vigour;
    signed int attack;
    signed int defense;
    signed int evasion;
    float attack_speed;
    float positionX, positionY;
    float dimensionX, dimensionY;
    float speed;
    float vectorX, vectorY;
    unsigned char R_color, G_color, B_color, Alpha;
    char name[32];
    enum GeneralType general_type;
    enum FatigueLevel fatigue_level;
    enum BattalionType battalion_type;
    Battalion *battalions;
    enum Sprites units_type;
} General;

#endif