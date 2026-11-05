#ifndef PROJECTILE_INTERNAL_H
#define PROJECTILE_INTERNAL_H

#include "projectile_types.h"
#include "battlefield_types.h"
#include <stdbool.h>

// what a shooter hands over when it looses an arrow; the projectile module does the aiming
typedef struct ProjectileLaunch {

    enum ArmyBattlefieldSide side;  // the army that fired, so the arrow only hurts the other one
    float x, y;
    float target_x, target_y;
    float target_vec_x, target_vec_y;  // how far the target moves per tick, used to lead the shot
    float damage;
    _Bool is_armor_piercing;

} ProjectileLaunch;

typedef struct Projectile {

    enum ArmyBattlefieldSide side;
    float start_x, start_y;
    float aim_x, aim_y;
    float elapsed;
    float flight_time;
    float damage;
    _Bool is_armor_piercing;

} Projectile;

#endif
