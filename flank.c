#include "flank.h"
#include "ai_order_internal.h"
#include "battalion_table_internal.h"
#include "battalion_types.h"
#include "general_table_internal.h"
#include "table_internal.h"
#include "target_types.h"
#include "unit_table_internal.h"
#include <math.h>
#include <stdbool.h>

#define PI 3.14159274f
#define DIR_STEPS 4.0f            // facing values for a full circle, same as formation.c
#define FLANK_FRONT_COS 0.5f      // attacks within 60 degrees of the facing hit the front
#define FLANK_REAR_COS -0.5f      // attacks within 60 degrees of the back hit the rear
#define FLANK_MIN_CONTACT 5u      // units that must be fighting the target before the attack counts
#define FLANK_SIDE_HIT_BONUS 10.0f
#define FLANK_REAR_HIT_BONUS 20.0f
#define FLANK_SIDE_DAMAGE 1.25f
#define FLANK_REAR_DAMAGE 1.5f
#define CAVALRY_FLANK_DAMAGE 1.25f // on top of the above, so riders seek the flanks instead of the front

// private prototypes
static void copy_facing(Table *tables, unsigned int battalion_count);
static void evaluate_attacks(Table *tables, unsigned int battalion_count, unsigned int battalion_size);
static unsigned int units_in_contact(const Table *tables, unsigned int attacker, unsigned int target, unsigned int battalion_size);
static enum FlankState classify_angle(const Table *tables, unsigned int attacker, unsigned int target);

// Flanking is decided per battalion, never per unit: only a battalion ordered to attack another one,
// and already fighting it, flanks it. Units that bump into a battalion by accident never do.
void flank_update(Table *player_tables, Table *enemy_tables, unsigned int battalion_count, unsigned int battalion_size) {

    copy_facing(player_tables, battalion_count);
    copy_facing(enemy_tables, battalion_count);

    evaluate_attacks(player_tables, battalion_count, battalion_size);
    evaluate_attacks(enemy_tables, battalion_count, battalion_size);

}

float flank_hit_bonus(enum FlankState flank) {

    if (flank == FLANK_SIDE) return FLANK_SIDE_HIT_BONUS;
    if (flank == FLANK_REAR) return FLANK_REAR_HIT_BONUS;
    return 0.0f;

}

float flank_damage_multiplier(enum FlankState flank, _Bool is_cavalry) {

    float multiplier = 1.0f;
    if (flank == FLANK_SIDE) multiplier = FLANK_SIDE_DAMAGE;
    if (flank == FLANK_REAR) multiplier = FLANK_REAR_DAMAGE;
    if (flank != FLANK_NONE && is_cavalry) multiplier *= CAVALRY_FLANK_DAMAGE;
    return multiplier;

}

// a battalion faces the way its general last turned it, and keeps that facing once headless
static void copy_facing(Table *tables, unsigned int battalion_count) {

    for (unsigned int b = 0; b < battalion_count; b++) {

        tables->battalion_tables.friendly_dynamic.facing[b] = tables->general_tables.friendly_dynamic.facing[b];

    }

}

// tables is the attacking army: writes its flank_attack and the enemy's flank_exposure
static void evaluate_attacks(Table *tables, unsigned int battalion_count, unsigned int battalion_size) {

    BattalionDynamicTable *own = &tables->battalion_tables.friendly_dynamic;
    BattalionDynamicTable *enemy = &tables->battalion_tables.enemy_dynamic;
    const BattalionOrderTable *order = &tables->battalion_order;

    for (unsigned int b = 0; b < battalion_count; b++) {

        enemy->flank_exposure[b] = FLANK_NONE;

    }

    for (unsigned int a = 0; a < battalion_count; a++) {

        own->flank_attack[a] = FLANK_NONE;

        if (!own->is_present[a] || !own->is_controllable[a]) continue;
        if (order->type[a] != ORDER_ATTACK) continue;
        if (tables->battalion_tables.friendly_static.is_ranged[a]) continue;

        const unsigned int target = order->target[a];
        if (target >= battalion_count || !enemy->is_present[target]) continue;
        if (units_in_contact(tables, a, target, battalion_size) < FLANK_MIN_CONTACT) continue;

        const enum FlankState flank = classify_angle(tables, a, target);
        own->flank_attack[a] = flank;

        // only the worst exposure counts, so several flankers never stack their penalties
        if (flank > enemy->flank_exposure[target]) enemy->flank_exposure[target] = flank;

    }

}

static unsigned int units_in_contact(const Table *tables, unsigned int attacker, unsigned int target, unsigned int battalion_size) {

    const UnitDynamicTable *units = &tables->unit_tables.friendly_dynamic;
    unsigned int count = 0;

    for (unsigned int k = 0; k < battalion_size; k++) {

        const unsigned int idx = attacker * battalion_size + k;
        if (!units->is_present[idx] || !units->is_engaged[idx] || !units->is_target_in_range[idx]) continue;

        const unsigned int target_idx = units->target_idx[idx];
        const unsigned int target_battalion = (units->target_type[idx] == TARGET_UNIT) ? target_idx / battalion_size : target_idx;
        if (target_battalion == target) count++;

    }

    return count;

}

static enum FlankState classify_angle(const Table *tables, unsigned int attacker, unsigned int target) {

    const BattalionDynamicTable *own = &tables->battalion_tables.friendly_dynamic;
    const BattalionDynamicTable *enemy = &tables->battalion_tables.enemy_dynamic;

    const float dx = own->x[attacker] - enemy->x[target];
    const float dy = own->y[attacker] - enemy->y[target];
    const float length = sqrtf(dx * dx + dy * dy);
    if (length < 0.0001f) return FLANK_NONE;

    const float angle = enemy->facing[target] * (2.0f * PI / DIR_STEPS);
    const float cos_to_attacker = (dx * cosf(angle) + dy * sinf(angle)) / length;

    if (cos_to_attacker >= FLANK_FRONT_COS) return FLANK_NONE;
    if (cos_to_attacker <= FLANK_REAR_COS) return FLANK_REAR;
    return FLANK_SIDE;

}
