#ifndef SPREADSHEET_INTERNAL_H
#define SPREADSHEET_INTERNAL_H

#include "spreadsheet_types.h"
#include "battalion_types.h"
#include "general_types.h"
#include "sprites_types.h"

// rulebook base type stats

typedef struct BattalionSpreadsheet {

    const enum BattalionType battalion_type;
    const enum Sprites sprite_type;
    
    const float morale;
    const float fatigue;
    const float hp;
    const float speed;
    const float meele_attack;
    const float meele_defense;
    const float strength;    
    const float range;
    const float charge_bonus;
    const float armor;
    const float attack_speed;

    const unsigned char formation_x;
    const unsigned char formation_y;

    const _Bool shielded;
    const _Bool ranged;
    const _Bool large;
    const _Bool armor_piercing;
    const _Bool anti_large;

} BattalionSpreadsheet;

typedef struct GeneralSpreadSheet {

    const enum GeneralType type;
    const enum Sprites sprite_type;
    
    const float hp;
    const float meele_attack;
    const float meele_defense;
    const float strength;    
    const float range;
    const float charge_bonus;
    const float armor;
    const float attack_speed;

    const _Bool shielded;
    const _Bool ranged;
    const _Bool large;
    const _Bool armor_piercing;
    const _Bool anti_large;

} GeneralSpreadSheet;

typedef struct Spreadsheet {

    BattalionSpreadsheet battalion_spreadsheet[BATTALION_TYPE_COUNT];

} Spreadsheet;

#endif