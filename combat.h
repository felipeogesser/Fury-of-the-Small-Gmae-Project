#ifndef COMBAT_H
#define COMBAT_H

float apply_armor(float damage, float armor);
float hit_chance(float melee_attack, float melee_defense);
_Bool roll_hit(float chance);

#endif