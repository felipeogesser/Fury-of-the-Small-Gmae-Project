#include "combat.h"
#include <math.h>

float apply_armor(float damage, float armor) {

    #define ARMOR_K 100.0f

    float multiplier = ARMOR_K / (armor + ARMOR_K);
    float result = damage * multiplier;
    return result;

    #undef ARMOR_K

}

float hit_chance(float melee_attack, float melee_defense) {

    #define BASE_HIT = 35.0f
    #define MIN_HIT = 8.0f
    #define MAX_HIT = 90.0f
    
    float chance = BASE_HIT + meele_attack - meele_defense;
    chance = MIN_HIT * (chance < MIN_HIT) + MAX_HIT * (chance > MAX_HIT);
    return chance;

    #undef BASE_HIT
    #undef MIN_HIT
    #undef MAX_HIT

}

_Bool roll_hit(float chance) {
    
    float roll = (float)(rand() % 100 + 1);
    return roll >= chance;

}