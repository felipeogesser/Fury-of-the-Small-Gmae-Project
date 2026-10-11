#ifndef GENERAL_INTERNAL_H
#define GENERAL_INTERNAL_H

#include "general_types.h"
#include "animation_internal.h"
#include "battalion_types.h"
#include "sprites_internal.h"

typedef struct General {
    AnimationState anim;
    SpriteInfo sprite;
    unsigned int id;
    enum Rarity rarity;
    unsigned char drawer_slot_index;
    _Bool render;
    signed int hp;
    signed int vigour;//
    signed int base_meele_attack;
    signed int base_meele_defense;
    signed int armor;
    float strength;
    float attack_speed;
    float positionX, positionY;
    float dimensionX, dimensionY;
    float speed;
    float vectorX, vectorY;
    unsigned char R_color, G_color, B_color, Alpha;
    char name[32];
    enum GeneralType general_type;
    //enum FatigueLevel fatigue_level;
    enum BattalionType battalion_type;
    Battalion *battalions;
    enum Sprites units_type;

    float range;
    float charge_bonus;

    _Bool shielded;
    _Bool ranged;
    _Bool large;
    _Bool armor_piercing;
    _Bool anti_large;

} General;

#endif