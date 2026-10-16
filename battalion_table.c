#include "battalion_table.h"
#include "battalion_table_internal.h"
#include "battalion_policy.h"
#include "battalion_types.h"
#include "ai_order.h"
#include "ai_order_internal.h"
#include "aos_soa_transformer.h"
#include "fatigue.h"
#include "fatigue_internal.h"
#include "field_entry.h"
#include "field_entry_internal.h"
#include "memory_arena.h"
#include "morale.h"
#include "morale_internal.h"
#include "spreadsheet_internal.h"
#include "table_internal.h"
#include "threat_types.h"
#include "threat_policy.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// private prototypes
static _Bool check_if_battalion_is_present(const Table *tables, unsigned int i);
static void update_counts(Table *tables, unsigned int i, unsigned int battalion_size);
static enum BattalionLifecycle evaluate_battalion_lifecycle(Table *tables, unsigned int i);
static _Bool check_if_battalion_is_controllable(enum BattalionLifecycle lifecycle_state);
static void send_order_to_general(Table *tables, _Bool is_controllable, unsigned int i);
static enum BattalionMovementMode evaluate_battalion_movement(Table *tables, unsigned int i);
static _Bool evaluate_charge_timer(float *charge_timer, float dt);
static _Bool check_if_battalion_is_active(enum BattalionMovementMode movement_state);
static void speed_update(float base_speed, float *speed, enum FatigueState state);
static void hp_update(const Table *tables, float *hp, unsigned int i, unsigned int battalion_size);
static void position_update(const Table *tables, float *x, float *y, unsigned int i, unsigned int battalion_size);
static void update_stats(const Spreadsheet *spreadsheet, Table *tables, unsigned int i);
static _Bool check_if_battalion_can_charge(Table *tables, enum BattalionMovementMode movement_state, unsigned int i);
static _Bool check_if_battalion_is_engaged(enum BattalionMovementMode movement_state);
static void allocate_mock_soa_memory(
    unsigned int obj_count,
    BattalionStaticTable *mock_friendly_static_table,
    BattalionStaticTable *mock_enemy_static_table,
    BattalionStaticTableDiff *mock_friendly_static_table_diff,
    BattalionStaticTableDiff *mock_enemy_static_table_diff,
    BattalionDynamicTable *mock_friendly_dynamic_table,
    BattalionDynamicTable *mock_enemy_dynamic_table,
    BattalionDynamicTableDiff *mock_friendly_dynamic_table_diff,
    BattalionDynamicTableDiff *mock_enemy_dynamic_table_diff);
static void allocate_mem_for_mock(unsigned int obj_count, const ComplexFieldEntry *field_table, void *mock);
static void battalion_static_table_init(
    Battalion *battalion,
    BattalionStaticTable *mock_static_table,
    const ComplexFieldEntry *battalion_field_table,
    const ComplexFieldEntry *mock_static_field_table,
    unsigned int battalion_count);
static void battalion_dynamic_table_init(
    Battalion *battalion,
    BattalionDynamicTable *mock_dynamic_table,
    const ComplexFieldEntry *battalion_field_table,
    const ComplexFieldEntry *mock_dynamic_field_table,
    unsigned int battalion_count);
static void battalion_static_diff_table_init(
    unsigned int obj_count,
    BattalionStaticTableDiff *mock_static_table_diff,
    BattalionStaticTable *mock_friendly_static_table,
    BattalionStaticTable *mock_enemy_static_table,
    const ComplexFieldEntry *mock_static_field_table_diff,
    const ComplexFieldEntry *mock_static_field_table);
static void battalion_dynamic_diff_table_init(
    unsigned int obj_count,
    BattalionDynamicTableDiff *mock_dynamic_table_diff,
    BattalionDynamicTable *mock_friendly_dynamic_table,
    BattalionDynamicTable *mock_enemy_dynamic_table,
    const ComplexFieldEntry *mock_dynamic_field_table_diff,
    const ComplexFieldEntry *mock_dynamic_field_table);

static void *battalion_table_start_address_alloc = NULL;

#define OFFSET_OF(type, member) offsetof(type, member)
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))
#define ELEMENT_SIZE_OF(type, member) (sizeof(*(((type *)0)->member)))

static const ComplexFieldEntry battalion_static_table_field_table[] = {

    { "battalion_type",     OFFSET_OF(BattalionStaticTable, battalion_type),     ELEMENT_SIZE_OF(BattalionStaticTable, battalion_type),     S_INT,  NULL },
    { "range",              OFFSET_OF(BattalionStaticTable, range),              ELEMENT_SIZE_OF(BattalionStaticTable, range),              FLOAT,  NULL },
    { "charge_bonus",       OFFSET_OF(BattalionStaticTable, charge_bonus),       ELEMENT_SIZE_OF(BattalionStaticTable, charge_bonus),       FLOAT,  NULL },
    { "armor",              OFFSET_OF(BattalionStaticTable, armor),              ELEMENT_SIZE_OF(BattalionStaticTable, armor),              FLOAT,  NULL },
    { "attack_speed",       OFFSET_OF(BattalionStaticTable, attack_speed),       ELEMENT_SIZE_OF(BattalionStaticTable, attack_speed),       FLOAT,  NULL },
    { "start_entity_count", OFFSET_OF(BattalionStaticTable, start_entity_count), ELEMENT_SIZE_OF(BattalionStaticTable, start_entity_count), U_INT,  NULL },
    { "formation_x",        OFFSET_OF(BattalionStaticTable, formation_x),        ELEMENT_SIZE_OF(BattalionStaticTable, formation_x),        U_CHAR, NULL },
    { "formation_y",        OFFSET_OF(BattalionStaticTable, formation_y),        ELEMENT_SIZE_OF(BattalionStaticTable, formation_y),        U_CHAR, NULL },
    { "is_shielded",           OFFSET_OF(BattalionStaticTable, is_shielded),           ELEMENT_SIZE_OF(BattalionStaticTable, is_shielded),           BOOL,   NULL },
    { "is_ranged",             OFFSET_OF(BattalionStaticTable, is_ranged),             ELEMENT_SIZE_OF(BattalionStaticTable, is_ranged),             BOOL,   NULL },
    { "is_large",              OFFSET_OF(BattalionStaticTable, is_large),              ELEMENT_SIZE_OF(BattalionStaticTable, is_large),              BOOL,   NULL },
    { "is_armor_piercing",     OFFSET_OF(BattalionStaticTable, is_armor_piercing),     ELEMENT_SIZE_OF(BattalionStaticTable, is_armor_piercing),     BOOL,   NULL },
    { "is_anti_large",         OFFSET_OF(BattalionStaticTable, is_anti_large),         ELEMENT_SIZE_OF(BattalionStaticTable, is_anti_large),         BOOL,   NULL },
    { "obj_size", 0, sizeof(BattalionStaticTable), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const battalion_static_table_field_table_ptr = battalion_static_table_field_table;

static const ComplexFieldEntry battalion_static_table_diff_field_table[] = {

    { "diff_armor", OFFSET_OF(BattalionStaticTableDiff, diff_armor), ELEMENT_SIZE_OF(BattalionStaticTableDiff, diff_armor), FLOAT, NULL },
    { "obj_size", 0, sizeof(BattalionStaticTableDiff), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const battalion_static_table_diff_field_table_ptr = battalion_static_table_diff_field_table;

static const ComplexFieldEntry battalion_dynamic_table_field_table[] = {

    { "headless_movement_mode",  OFFSET_OF(BattalionDynamicTable, headless_movement_mode),  ELEMENT_SIZE_OF(BattalionDynamicTable, headless_movement_mode),  S_INT, NULL },
    { "fatigue_state",           OFFSET_OF(BattalionDynamicTable, fatigue_state),           ELEMENT_SIZE_OF(BattalionDynamicTable, fatigue_state),           S_INT, NULL },
    { "morale_state",            OFFSET_OF(BattalionDynamicTable, morale_state),            ELEMENT_SIZE_OF(BattalionDynamicTable, morale_state),            S_INT, NULL },
    { "lifecycle_state",         OFFSET_OF(BattalionDynamicTable, lifecycle_state),         ELEMENT_SIZE_OF(BattalionDynamicTable, lifecycle_state),         S_INT, NULL },
    { "movement_state",          OFFSET_OF(BattalionDynamicTable, movement_state),          ELEMENT_SIZE_OF(BattalionDynamicTable, movement_state),          S_INT, NULL },
    { "movement_pace",           OFFSET_OF(BattalionDynamicTable, movement_pace),           ELEMENT_SIZE_OF(BattalionDynamicTable, movement_pace),           S_INT, NULL },
    { "target_battalion_idx",    OFFSET_OF(BattalionDynamicTable, target_battalion_idx),    ELEMENT_SIZE_OF(BattalionDynamicTable, target_battalion_idx),    U_INT, NULL },
    { "morale_modifiers",        OFFSET_OF(BattalionDynamicTable, morale_modifiers),        ELEMENT_SIZE_OF(BattalionDynamicTable, morale_modifiers),        U_INT, NULL },
    { "withdrawed_entity_count", OFFSET_OF(BattalionDynamicTable, withdrawed_entity_count), ELEMENT_SIZE_OF(BattalionDynamicTable, withdrawed_entity_count), U_INT, NULL },
    { "dead_entity_count",       OFFSET_OF(BattalionDynamicTable, dead_entity_count),       ELEMENT_SIZE_OF(BattalionDynamicTable, dead_entity_count),       U_INT, NULL },
    { "alive_entity_count",      OFFSET_OF(BattalionDynamicTable, alive_entity_count),      ELEMENT_SIZE_OF(BattalionDynamicTable, alive_entity_count),      U_INT, NULL },
    { "unit_alive_count",        OFFSET_OF(BattalionDynamicTable, unit_alive_count),        ELEMENT_SIZE_OF(BattalionDynamicTable, unit_alive_count),        U_INT, NULL },
    { "target_x",                OFFSET_OF(BattalionDynamicTable, target_x),                ELEMENT_SIZE_OF(BattalionDynamicTable, target_x),                FLOAT, NULL },
    { "target_y",                OFFSET_OF(BattalionDynamicTable, target_y),                ELEMENT_SIZE_OF(BattalionDynamicTable, target_y),                FLOAT, NULL },
    { "x",                       OFFSET_OF(BattalionDynamicTable, x),                       ELEMENT_SIZE_OF(BattalionDynamicTable, x),                       FLOAT, NULL },
    { "y",                       OFFSET_OF(BattalionDynamicTable, y),                       ELEMENT_SIZE_OF(BattalionDynamicTable, y),                       FLOAT, NULL },
    { "morale",                  OFFSET_OF(BattalionDynamicTable, morale),                  ELEMENT_SIZE_OF(BattalionDynamicTable, morale),                  FLOAT, NULL },
    { "fatigue",                 OFFSET_OF(BattalionDynamicTable, fatigue),                 ELEMENT_SIZE_OF(BattalionDynamicTable, fatigue),                 FLOAT, NULL },
    { "hp",                      OFFSET_OF(BattalionDynamicTable, hp),                      ELEMENT_SIZE_OF(BattalionDynamicTable, hp),                      FLOAT, NULL },
    { "speed",                   OFFSET_OF(BattalionDynamicTable, speed),                   ELEMENT_SIZE_OF(BattalionDynamicTable, speed),                   FLOAT, NULL },
    { "meele_attack",            OFFSET_OF(BattalionDynamicTable, meele_attack),            ELEMENT_SIZE_OF(BattalionDynamicTable, meele_attack),            FLOAT, NULL },
    { "meele_defense",           OFFSET_OF(BattalionDynamicTable, meele_defense),           ELEMENT_SIZE_OF(BattalionDynamicTable, meele_defense),           FLOAT, NULL },
    { "strength",                OFFSET_OF(BattalionDynamicTable, strength),                ELEMENT_SIZE_OF(BattalionDynamicTable, strength),                FLOAT, NULL },
    { "rally_timer",             OFFSET_OF(BattalionDynamicTable, rally_timer),             ELEMENT_SIZE_OF(BattalionDynamicTable, rally_timer),             FLOAT, NULL },
    { "charge_timer",            OFFSET_OF(BattalionDynamicTable, charge_timer),            ELEMENT_SIZE_OF(BattalionDynamicTable, charge_timer),            FLOAT, NULL },
    { "is_active",               OFFSET_OF(BattalionDynamicTable, is_active),               ELEMENT_SIZE_OF(BattalionDynamicTable, is_active),               BOOL,  NULL },
    { "is_rallying",             OFFSET_OF(BattalionDynamicTable, is_rallying),             ELEMENT_SIZE_OF(BattalionDynamicTable, is_rallying),             BOOL,  NULL },
    { "is_charging",             OFFSET_OF(BattalionDynamicTable, is_charging),             ELEMENT_SIZE_OF(BattalionDynamicTable, is_charging),             BOOL,  NULL },
    { "is_engaging",             OFFSET_OF(BattalionDynamicTable, is_engaging),             ELEMENT_SIZE_OF(BattalionDynamicTable, is_engaging),             BOOL,  NULL },
    { "is_engaged",              OFFSET_OF(BattalionDynamicTable, is_engaged),              ELEMENT_SIZE_OF(BattalionDynamicTable, is_engaged),              BOOL,  NULL },
    { "is_controllable",         OFFSET_OF(BattalionDynamicTable, is_controllable),         ELEMENT_SIZE_OF(BattalionDynamicTable, is_controllable),         BOOL,  NULL },
    { "is_present",              OFFSET_OF(BattalionDynamicTable, is_present),              ELEMENT_SIZE_OF(BattalionDynamicTable, is_present),              BOOL,  NULL },
    { "is_headless",             OFFSET_OF(BattalionDynamicTable, is_headless),             ELEMENT_SIZE_OF(BattalionDynamicTable, is_headless),             BOOL,  NULL },
    { "is_bracing",              OFFSET_OF(BattalionDynamicTable, is_bracing),              ELEMENT_SIZE_OF(BattalionDynamicTable, is_bracing),              BOOL,  NULL },
    { "obj_size", 0, sizeof(BattalionDynamicTable), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const battalion_dynamic_table_field_table_ptr = battalion_dynamic_table_field_table;


static const ComplexFieldEntry battalion_dynamic_table_diff_field_table[] = {

    { "threat",             OFFSET_OF(BattalionDynamicTableDiff, threat),             ELEMENT_SIZE_OF(BattalionDynamicTableDiff, threat),             S_INT, NULL },
    { "dx",                 OFFSET_OF(BattalionDynamicTableDiff, dx),                 ELEMENT_SIZE_OF(BattalionDynamicTableDiff, dx),                 FLOAT, NULL },
    { "dy",                 OFFSET_OF(BattalionDynamicTableDiff, dy),                 ELEMENT_SIZE_OF(BattalionDynamicTableDiff, dy),                 FLOAT, NULL },
    { "distance",           OFFSET_OF(BattalionDynamicTableDiff, distance),           ELEMENT_SIZE_OF(BattalionDynamicTableDiff, distance),           FLOAT, NULL },
    { "diff_morale",        OFFSET_OF(BattalionDynamicTableDiff, diff_morale),        ELEMENT_SIZE_OF(BattalionDynamicTableDiff, diff_morale),        FLOAT, NULL },
    { "diff_fatigue",       OFFSET_OF(BattalionDynamicTableDiff, diff_fatigue),       ELEMENT_SIZE_OF(BattalionDynamicTableDiff, diff_fatigue),       FLOAT, NULL },
    { "diff_hp",            OFFSET_OF(BattalionDynamicTableDiff, diff_hp),            ELEMENT_SIZE_OF(BattalionDynamicTableDiff, diff_hp),            FLOAT, NULL },
    { "diff_speed",         OFFSET_OF(BattalionDynamicTableDiff, diff_speed),         ELEMENT_SIZE_OF(BattalionDynamicTableDiff, diff_speed),         FLOAT, NULL },
    { "diff_meele_attack",  OFFSET_OF(BattalionDynamicTableDiff, diff_meele_attack),  ELEMENT_SIZE_OF(BattalionDynamicTableDiff, diff_meele_attack),  FLOAT, NULL },
    { "diff_meele_defense", OFFSET_OF(BattalionDynamicTableDiff, diff_meele_defense), ELEMENT_SIZE_OF(BattalionDynamicTableDiff, diff_meele_defense), FLOAT, NULL },
    { "diff_strength",      OFFSET_OF(BattalionDynamicTableDiff, diff_strength),      ELEMENT_SIZE_OF(BattalionDynamicTableDiff, diff_strength),      FLOAT, NULL },
    { "obj_size", 0, sizeof(BattalionDynamicTableDiff), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const battalion_dynamic_table_diff_field_table_ptr = battalion_dynamic_table_diff_field_table;

/*static const ComplexFieldEntry battalion_comparison_table_field_table[] = {

    { "friendly_static",       OFFSET_OF(BattalionComparisonTable, friendly_static),       SIZE_OF(BattalionComparisonTable, friendly_static),       STRUCT, battalion_static_table_field_table },
    { "enemy_static",          OFFSET_OF(BattalionComparisonTable, enemy_static),          SIZE_OF(BattalionComparisonTable, enemy_static),          STRUCT, battalion_static_table_field_table },
    { "friendly_static_diff",  OFFSET_OF(BattalionComparisonTable, friendly_static_diff),  SIZE_OF(BattalionComparisonTable, friendly_static_diff),  STRUCT, battalion_static_table_diff_field_table },
    { "enemy_static_diff",     OFFSET_OF(BattalionComparisonTable, enemy_static_diff),     SIZE_OF(BattalionComparisonTable, enemy_static_diff),     STRUCT, battalion_static_table_diff_field_table },
    { "friendly_dynamic",      OFFSET_OF(BattalionComparisonTable, friendly_dynamic),      SIZE_OF(BattalionComparisonTable, friendly_dynamic),      STRUCT, battalion_dynamic_table_field_table },
    { "enemy_dynamic",         OFFSET_OF(BattalionComparisonTable, enemy_dynamic),         SIZE_OF(BattalionComparisonTable, enemy_dynamic),         STRUCT, battalion_dynamic_table_field_table },
    { "friendly_dynamic_diff", OFFSET_OF(BattalionComparisonTable, friendly_dynamic_diff), SIZE_OF(BattalionComparisonTable, friendly_dynamic_diff), STRUCT, battalion_dynamic_table_diff_field_table },
    { "enemy_dynamic_diff",    OFFSET_OF(BattalionComparisonTable, enemy_dynamic_diff),    SIZE_OF(BattalionComparisonTable, enemy_dynamic_diff),    STRUCT, battalion_dynamic_table_diff_field_table },
    { "obj_size", 0, sizeof(BattalionComparisonTable), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const battalion_comparison_table_field_table_ptr = battalion_comparison_table_field_table;*/

#undef OFFSET_OF
#undef SIZE_OF
#undef ELEMENT_SIZE_OF

void battalion_table_init(Table *player_tables, Table *enemy_tables, Battalion *player_battalion, Battalion *enemy_battalion,/* const ComplexFieldEntry *battalion_field_table,*/ unsigned int battalion_count) {

    BattalionStaticTable mock_friendly_static_table = {0};
    BattalionStaticTable mock_enemy_static_table = {0};
    BattalionStaticTableDiff mock_friendly_static_table_diff = {0};
    BattalionStaticTableDiff mock_enemy_static_table_diff = {0};
    BattalionDynamicTable mock_friendly_dynamic_table = {0};
    BattalionDynamicTable mock_enemy_dynamic_table = {0};
    BattalionDynamicTableDiff mock_friendly_dynamic_table_diff = {0};
    BattalionDynamicTableDiff mock_enemy_dynamic_table_diff = {0};

    allocate_mock_soa_memory(
        battalion_count,
        &mock_friendly_static_table,
        &mock_enemy_static_table,
        &mock_friendly_static_table_diff,
        &mock_enemy_static_table_diff,
        &mock_friendly_dynamic_table,
        &mock_enemy_dynamic_table,
        &mock_friendly_dynamic_table_diff,
        &mock_enemy_dynamic_table_diff
    );

    battalion_static_table_init(player_battalion, &mock_friendly_static_table, battalion_field_table_ptr, battalion_static_table_field_table_ptr, battalion_count);
    battalion_static_table_init(enemy_battalion, &mock_enemy_static_table, battalion_field_table_ptr, battalion_static_table_field_table_ptr, battalion_count);
    
    battalion_dynamic_table_init(player_battalion, &mock_friendly_dynamic_table, battalion_field_table_ptr, battalion_dynamic_table_field_table_ptr, battalion_count);
    battalion_dynamic_table_init(enemy_battalion, &mock_enemy_dynamic_table, battalion_field_table_ptr, battalion_dynamic_table_field_table_ptr, battalion_count);
    
    battalion_static_diff_table_init(battalion_count, &mock_friendly_static_table_diff, &mock_friendly_static_table, &mock_enemy_static_table, battalion_static_table_diff_field_table_ptr, battalion_static_table_field_table_ptr);
    battalion_dynamic_diff_table_init(battalion_count, &mock_friendly_dynamic_table_diff, &mock_friendly_dynamic_table, &mock_enemy_dynamic_table, battalion_dynamic_table_diff_field_table_ptr, battalion_dynamic_table_field_table_ptr);

    battalion_static_diff_table_init(battalion_count, &mock_enemy_static_table_diff, &mock_enemy_static_table, &mock_friendly_static_table, battalion_static_table_diff_field_table_ptr, battalion_static_table_field_table_ptr);
    battalion_dynamic_diff_table_init(battalion_count, &mock_enemy_dynamic_table_diff, &mock_enemy_dynamic_table, &mock_friendly_dynamic_table, battalion_dynamic_table_diff_field_table_ptr, battalion_dynamic_table_field_table_ptr);

    player_tables->battalion_tables.friendly_static = mock_friendly_static_table;
    player_tables->battalion_tables.enemy_static = mock_enemy_static_table;
    enemy_tables->battalion_tables.friendly_static = mock_enemy_static_table;
    enemy_tables->battalion_tables.enemy_static = mock_friendly_static_table;

    player_tables->battalion_tables.friendly_dynamic = mock_friendly_dynamic_table;
    player_tables->battalion_tables.enemy_dynamic = mock_enemy_dynamic_table;
    enemy_tables->battalion_tables.friendly_dynamic = mock_enemy_dynamic_table;
    enemy_tables->battalion_tables.enemy_dynamic = mock_friendly_dynamic_table;

    player_tables->battalion_tables.friendly_static_diff = mock_friendly_static_table_diff;
    player_tables->battalion_tables.friendly_dynamic_diff = mock_friendly_dynamic_table_diff;

    enemy_tables->battalion_tables.friendly_static_diff = mock_enemy_static_table_diff;
    enemy_tables->battalion_tables.friendly_dynamic_diff = mock_enemy_dynamic_table_diff;

}

void battalion_table_update(const Spreadsheet *spreadsheet, Table *tables, signed int balance_of_power, unsigned int battalion_count, unsigned int battalion_size, float dt) {

    for (unsigned int i = 0; i < battalion_count; i++) {

        _Bool is_present = check_if_battalion_is_present(tables, i);
        tables->battalion_tables.friendly_dynamic.is_present[i] = is_present;
        if (!is_present) continue;

        update_counts(tables, i, battalion_size);

        enum BattalionLifecycle lifecycle_state = evaluate_battalion_lifecycle(tables, i);
        tables->battalion_tables.friendly_dynamic.lifecycle_state[i] = lifecycle_state;

        _Bool is_controllable = check_if_battalion_is_controllable(lifecycle_state);
        tables->battalion_tables.friendly_dynamic.is_controllable[i] = is_controllable;

        send_order_to_general(tables, is_controllable, i);

        update_rally(tables, i, dt);

        enum BattalionMovementMode movement_state = tables->battalion_tables.friendly_dynamic.movement_state[i];
        if (movement_state != CHARGING) {
            movement_state = evaluate_battalion_movement(tables, i);
            tables->battalion_tables.friendly_dynamic.movement_state[i] = movement_state;
        } else {
            _Bool is_charging = evaluate_charge_timer(&tables->battalion_tables.friendly_dynamic.charge_timer[i], dt);
            if (!is_charging) {
                tables->battalion_tables.friendly_dynamic.charge_timer[i] = 0.0f;
                tables->battalion_tables.friendly_dynamic.is_charging[i] = false;
                movement_state = FIGHTING;
                tables->battalion_tables.friendly_dynamic.movement_state[i] = movement_state;
            }
        }

        _Bool is_engaged = check_if_battalion_is_engaged(movement_state);
        tables->battalion_tables.friendly_dynamic.is_engaged[i] = is_engaged;

        _Bool is_active = check_if_battalion_is_active(movement_state);
        tables->battalion_tables.friendly_dynamic.is_present[i] = is_active;

        enum BattalionType battalion_type = tables->battalion_tables.friendly_static.battalion_type[i];
        Fatigue fatigue = {
            .state = tables->battalion_tables.friendly_dynamic.fatigue_state[i],
            .base = spreadsheet->battalion_spreadsheet[battalion_type].fatigue,
            .current = tables->battalion_tables.friendly_dynamic.fatigue[i]
        };
        _Bool is_fighting = (tables->battalion_tables.friendly_dynamic.movement_state[i] == FIGHTING);
        fatigue_update(&fatigue, is_active, is_fighting, dt);
        tables->battalion_tables.friendly_dynamic.fatigue_state[i] = fatigue.state;
        tables->battalion_tables.friendly_dynamic.fatigue[i] = fatigue.current;

        float base_speed = spreadsheet->battalion_spreadsheet[battalion_type].speed;
        float *speed = &tables->battalion_tables.friendly_dynamic.speed[i];
        speed_update(base_speed, speed, fatigue.state);

        Morale morale = {
            .state = tables->battalion_tables.friendly_dynamic.morale_state[i],
            .current = tables->battalion_tables.friendly_dynamic.morale[i],
            .base = spreadsheet->battalion_spreadsheet[battalion_type].morale,
            .modifiers = tables->battalion_tables.friendly_dynamic.morale_modifiers[i]
        };
        const HealthPool health_pool = {
            .max_hp = spreadsheet->battalion_spreadsheet[battalion_type].hp,
            .current_hp = tables->battalion_tables.friendly_dynamic.hp[i]
        };
        morale_update(&morale, &health_pool, balance_of_power);
        tables->battalion_tables.friendly_dynamic.morale_state[i] = morale.state;
        tables->battalion_tables.friendly_dynamic.morale[i] = morale.current;
        tables->battalion_tables.friendly_dynamic.morale_modifiers[i] = morale.modifiers;

        float *hp = &tables->battalion_tables.friendly_dynamic.hp[i];
        hp_update(tables, hp, i, battalion_size);

        float *x = &tables->battalion_tables.friendly_dynamic.x[i];
        float *y = &tables->battalion_tables.friendly_dynamic.y[i];
        position_update(tables, x, y, i, battalion_size);

        update_stats(spreadsheet, tables, i);

        is_present = check_if_battalion_is_present(tables, i); // Again...
        tables->battalion_tables.friendly_dynamic.is_present[i] = is_present;

    }

}

void battalion_table_destroy(void) {

    memory_arena_reset_to_address(battalion_table_start_address_alloc);

}

static _Bool check_if_battalion_is_present(const Table *tables, unsigned int i) {

    enum BattalionLifecycle lifecycle_state = tables->battalion_tables.friendly_dynamic.lifecycle_state[i];
    if (lifecycle_state == WIPED_OUT || lifecycle_state == WITHDRAWN) return false;
    return true;

}

static void update_counts(Table *tables, unsigned int i, unsigned int battalion_size) {

    unsigned int dead_entity_count = 0;
    unsigned int withdrawed_entity_count = 0;
    unsigned int alive_entity_count = 0;
    unsigned int unit_alive_count = 0;

    for (unsigned int k = i * battalion_size, j = 0; j < battalion_size; j++) {
        
        unsigned int idx = k + j;
        _Bool is_present = tables->unit_tables.friendly_dynamic.is_present[idx];
        float hp = tables->unit_tables.friendly_dynamic.hp[idx];
        _Bool is_dead = hp <= 0.0f;
        dead_entity_count += is_dead;
        withdrawed_entity_count += !is_present & !is_dead;
        alive_entity_count += !is_dead;
        unit_alive_count += !is_dead;
        
    }

    _Bool is_present = tables->general_tables.friendly_dynamic.is_present[i];
    float hp = tables->general_tables.friendly_dynamic.hp[i];
    _Bool is_dead = hp <= 0.0f;
    dead_entity_count += is_dead;
    withdrawed_entity_count += !is_present & !is_dead;
    alive_entity_count += !is_dead;

    tables->battalion_tables.friendly_dynamic.dead_entity_count[i] = dead_entity_count;
    tables->battalion_tables.friendly_dynamic.withdrawed_entity_count[i] = withdrawed_entity_count;
    tables->battalion_tables.friendly_dynamic.alive_entity_count[i] = alive_entity_count;
    tables->battalion_tables.friendly_dynamic.unit_alive_count[i] = unit_alive_count;

}

static enum BattalionLifecycle evaluate_battalion_lifecycle(Table *tables, unsigned int i) {

    unsigned int dead_entity_count = tables->battalion_tables.friendly_dynamic.dead_entity_count[i];
    unsigned int withdrawed_entity_count = tables->battalion_tables.friendly_dynamic.withdrawed_entity_count[i];
    unsigned int start_entity_count = tables->battalion_tables.friendly_static.start_entity_count[i];
    float hp = tables->battalion_tables.friendly_dynamic.hp[i];
    float morale = tables->battalion_tables.friendly_dynamic.morale[i];
    _Bool is_rallying = tables->battalion_tables.friendly_dynamic.is_rallying[i];
    if (hp <= 0.0f) return WIPED_OUT;
    if (start_entity_count == withdrawed_entity_count + dead_entity_count) return WITHDRAWN;
    if (is_rallying || morale <= 0.0f) return ROUTING;
    return NORMAL;

}

static _Bool check_if_battalion_is_controllable(enum BattalionLifecycle lifecycle_state) {

    if (lifecycle_state == NORMAL) return true;
    return false;

}

static void send_order_to_general(Table *tables, _Bool is_controllable, unsigned int i) {

    if (is_controllable) {

        const BattalionOrderTable order = tables->battalion_order;
        tables->general_tables.friendly_dynamic.order_x[i] = order.dest_x[i];
        tables->general_tables.friendly_dynamic.order_y[i] = order.dest_y[i];
        tables->battalion_tables.friendly_dynamic.target_battalion_idx[i] = order.target[i];

    } else {

        tables->general_tables.friendly_dynamic.order_x[i] = NAN;
        tables->general_tables.friendly_dynamic.order_y[i] = NAN;

    }

}

static enum BattalionMovementMode evaluate_battalion_movement(Table *tables, unsigned int i) {

    BattalionOrderTable order = tables->battalion_order;
    enum BattalionMovementMode movement_state = 0;
    if (order.type[i] == ORDER_HOLD) return STATIONARY;
    if (order.type[i] == ORDER_MOVE || order.type[i] == ORDER_ATTACK) {
        if (order.pace[i] == PACE_WALK) movement_state = WALKING;
        if (order.pace[i] == PACE_RUN) movement_state = RUNNING;
    }

    _Bool can_charge = check_if_battalion_can_charge(tables, movement_state, i);
    if (can_charge) {
        movement_state = CHARGING;
        tables->battalion_tables.friendly_dynamic.is_charging[i] = true;
    }
    /*_Bool can_fight = check_if_battalion_can_fight(tables, i);
    if (can_fight) movement_state = FIGHTING;*/
    
    return movement_state;

}

static _Bool evaluate_charge_timer(float *charge_timer, float dt) {

    #define CHARGE_TIMER 2.0f
    *charge_timer += dt;
    if (*charge_timer > CHARGE_TIMER) return false;
    return true;

}

static _Bool check_if_battalion_is_active(enum BattalionMovementMode movement_state) {

    if (movement_state == RUNNING || movement_state == CHARGING || movement_state == FIGHTING) return true;
    return false;
    
}


static void speed_update(float base_speed, float *speed, enum FatigueState state) {

    *speed = base_speed - (base_speed * 0.1f) * state;

}

static void hp_update(const Table *tables, float *hp, unsigned int i, unsigned int battalion_size) {

    float total_hp = 0.0f;
    for (unsigned int k = i * battalion_size, j = 0; j < battalion_size; j++) {

        unsigned int idx = k + j;
        float this_hp = tables->unit_tables.friendly_dynamic.hp[idx];
        total_hp += (this_hp <= 0.0f) ? 0.0f : this_hp;

    }

    float this_hp = tables->general_tables.friendly_dynamic.hp[i];
    total_hp += (this_hp <= 0.0f) ? 0.0f : this_hp;
    *hp = total_hp;

}

static void position_update(const Table *tables, float *x, float *y, unsigned int i, unsigned int battalion_size) {

    float average_x = 0.0f;
    float average_y = 0.0f;
    unsigned int not_present_count = 0;
    for (unsigned int k = i * battalion_size, j = 0; j < battalion_size; j++) {

        unsigned int idx = k + j;
        if(!tables->unit_tables.friendly_dynamic.is_present[idx]) {   
            not_present_count++;
            continue;
        }
        average_x += tables->unit_tables.friendly_dynamic.x[idx];
        average_y += tables->unit_tables.friendly_dynamic.y[idx];

    }
    _Bool general_present = tables->general_tables.friendly_dynamic.is_present[i];
    average_x += (general_present) ? tables->general_tables.friendly_dynamic.x[i] : 0.0f;
    average_y += (general_present) ? tables->general_tables.friendly_dynamic.y[i] : 0.0f;
    unsigned int entity_count = battalion_size + general_present - not_present_count;
    *x = average_x / entity_count;
    *y = average_y / entity_count;

}

static void update_stats(const Spreadsheet *spreadsheet, Table *tables, unsigned int i) {

    enum BattalionType battalion_type = tables->battalion_tables.friendly_static.battalion_type[i];
    enum MoraleState morale_state = tables->battalion_tables.friendly_dynamic.morale_state[i];
    enum FatigueState fatigue_state = tables->battalion_tables.friendly_dynamic.fatigue_state[i];
    const float base_meele_attack = spreadsheet->battalion_spreadsheet[battalion_type].meele_attack;
    const float base_meele_defense = spreadsheet->battalion_spreadsheet[battalion_type].meele_defense;
    const float base_strength = spreadsheet->battalion_spreadsheet[battalion_type].strength;
    float meele_attack = tables->battalion_tables.friendly_dynamic.meele_attack[i];
    float meele_defense = tables->battalion_tables.friendly_dynamic.meele_defense[i];
    float strength = tables->battalion_tables.friendly_dynamic.strength[i];

    meele_attack = base_meele_attack - (morale_state * 2 + fatigue_state * 3);
    meele_defense = base_meele_defense - (morale_state * 2 + fatigue_state * 3);
    strength = base_strength - (fatigue_state * 3);

    tables->battalion_tables.friendly_dynamic.meele_attack[i] = meele_attack;
    tables->battalion_tables.friendly_dynamic.meele_defense[i] = meele_defense;
    tables->battalion_tables.friendly_dynamic.strength[i] = strength;

}

static _Bool check_if_battalion_can_charge(Table *tables, enum BattalionMovementMode movement_state, unsigned int i) {

    _Bool is_engaging = tables->battalion_tables.friendly_dynamic.is_engaging[i];
    _Bool is_engaged = tables->battalion_tables.friendly_dynamic.is_engaged[i];
    if (!is_engaging || movement_state != RUNNING || movement_state == FIGHTING) return false;
    if (tables->battalion_order.type[i] != ORDER_ATTACK) return false;
    if (is_engaged) return false;

    _Bool is_charging = tables->battalion_tables.friendly_dynamic.is_charging[i];
    if (is_charging) return true;

    float x = tables->battalion_tables.friendly_dynamic.x[i];
    float y = tables->battalion_tables.friendly_dynamic.y[i];
    unsigned int target_idx = tables->battalion_tables.friendly_dynamic.target_battalion_idx[i];
    float target_x = tables->battalion_tables.enemy_dynamic.x[target_idx];
    float target_y = tables->battalion_tables.enemy_dynamic.y[target_idx];

    float dx = target_x - x;
    float dy = target_y - y;
    float dist = dx * dx + dy * dy;
    #define CHARGE_RANGE 50
    if (dist <= CHARGE_RANGE * CHARGE_RANGE) return true;
    #undef CHARGE_RANGE
    return false;
    
}

/*static _Bool check_if_battalion_can_fight(Table *tables, unsigned int i) {

    _Bool collision_with_enemy = tables->general_tables.friendly_dynamic.is_target_in_range[i];
    if (collision_with_enemy) return true;
    return false;

}*/

static _Bool check_if_battalion_is_engaged(enum BattalionMovementMode movement_state) {

    if (movement_state == FIGHTING) return true;
    return false;

}

static void allocate_mock_soa_memory(
    unsigned int obj_count,
    BattalionStaticTable *mock_friendly_static_table,
    BattalionStaticTable *mock_enemy_static_table,
    BattalionStaticTableDiff *mock_friendly_static_table_diff,
    BattalionStaticTableDiff *mock_enemy_static_table_diff,
    BattalionDynamicTable *mock_friendly_dynamic_table,
    BattalionDynamicTable *mock_enemy_dynamic_table,
    BattalionDynamicTableDiff *mock_friendly_dynamic_table_diff,
    BattalionDynamicTableDiff *mock_enemy_dynamic_table_diff) {

    battalion_table_start_address_alloc = memory_arena_current_pointer();
    allocate_mem_for_mock(obj_count, battalion_static_table_field_table_ptr, mock_friendly_static_table);
    allocate_mem_for_mock(obj_count, battalion_static_table_field_table_ptr, mock_enemy_static_table);
    allocate_mem_for_mock(obj_count, battalion_static_table_diff_field_table_ptr, mock_friendly_static_table_diff);
    allocate_mem_for_mock(obj_count, battalion_static_table_diff_field_table_ptr, mock_enemy_static_table_diff);
    allocate_mem_for_mock(obj_count, battalion_dynamic_table_field_table_ptr, mock_friendly_dynamic_table);
    allocate_mem_for_mock(obj_count, battalion_dynamic_table_field_table_ptr, mock_enemy_dynamic_table);
    allocate_mem_for_mock(obj_count, battalion_dynamic_table_diff_field_table_ptr, mock_friendly_dynamic_table_diff);
    allocate_mem_for_mock(obj_count, battalion_dynamic_table_diff_field_table_ptr, mock_enemy_dynamic_table_diff);

}

static void allocate_mem_for_mock(unsigned int obj_count, const ComplexFieldEntry *field_table, void *mock) {

    unsigned int fields_count = complex_field_table_fields_count(field_table);
    size_t size = 0;
    size_t alignment = 64;
    for (unsigned int i = 0; i < fields_count; i++) {

        size = field_table[i].size * obj_count;
        //alignment = size & -size;
        //alignment = (alignment <= 64) ? alignment : 64;

        size_t size_padded = (size + 63) & ~(size_t)63;
        void **dest = (void **)((char *)mock + field_table[i].offset);
        *dest = memory_arena_push(size_padded, alignment);

    }

}

static void battalion_static_table_init(
    Battalion *battalion,
    BattalionStaticTable *mock_static_table,
    const ComplexFieldEntry *battalion_field_table,
    const ComplexFieldEntry *mock_static_field_table,
    unsigned int battalion_count) {

    aos_soa_transform(battalion, (void **)mock_static_table, battalion_field_table, mock_static_field_table, battalion_count, 13,
    "battalion_type", "range", "charge_bonus", "armor", "attack_speed", "start_entity_count", "formation_x", "formation_y", "is_shielded", "is_ranged", "is_large", "is_armor_piercing", "is_anti_large");

}

static void battalion_dynamic_table_init(
    Battalion *battalion,
    BattalionDynamicTable *mock_dynamic_table,
    const ComplexFieldEntry *battalion_field_table,
    const ComplexFieldEntry *mock_dynamic_field_table,
    unsigned int battalion_count) {

    aos_soa_transform(battalion, (void **)mock_dynamic_table, battalion_field_table, mock_dynamic_field_table, battalion_count, 8,
    "morale", "fatigue", "unit_alive_count", "hp", "speed", "meele_attack", "meele_defense", "strength");

}

static void battalion_static_diff_table_init(
    unsigned int obj_count,
    BattalionStaticTableDiff *mock_static_table_diff,
    BattalionStaticTable *mock_friendly_static_table,
    BattalionStaticTable *mock_enemy_static_table,
    const ComplexFieldEntry *mock_static_field_table_diff,
    const ComplexFieldEntry *mock_static_field_table) {

    // this function assumes that the field tables are compatiable and counter parts of each other
    
    unsigned int diff_table_fields_count = complex_field_table_fields_count(mock_static_field_table_diff);
    const char *keys[diff_table_fields_count];
    memset(keys, 0 ,sizeof(const char *) * diff_table_fields_count);
    for (unsigned int i = 0; i < diff_table_fields_count; i++) {

        keys[i] = mock_static_field_table_diff[i].key;

    }

    unsigned int static_table_fields_count = complex_field_table_fields_count(mock_static_field_table);
    
    for (unsigned int o = 0; o < obj_count; o++) {
        
        for (unsigned int i = 0; i < diff_table_fields_count; i++) {

            for (unsigned int j = 0; j < static_table_fields_count; j++) {

                if (keys[i] == mock_static_field_table[j].key) {

                    void *src_friendly_ptr = (void *)((*((char **)((char *)mock_friendly_static_table + mock_static_field_table[j].offset))) + mock_static_field_table[j].size * o);
                    void *src_enemy_ptr = (void *)((*((char **)((char *)mock_enemy_static_table + mock_static_field_table[j].offset))) + mock_static_field_table[j].size * o);
                    
                    unsigned long long int src_friendly = *((unsigned long long int *)src_friendly_ptr);
                    unsigned long long int src_enemy = *((unsigned long long int *)src_enemy_ptr);
                    
                    size_t size = mock_static_field_table[j].size;
                    if (size < sizeof(unsigned long long int)) {

                        size_t shift = sizeof(unsigned long long int) - size;

                        src_friendly &= (-1ULL >> shift);
                        src_enemy &= (-1ULL >> shift);

                    }

                    void *dest = (void *)((*((char **)((char *)mock_static_table_diff + mock_static_field_table_diff[j].offset))) + mock_static_field_table_diff[j].size * o);
                    subtract_type(
                        dest, &src_friendly, &src_enemy,
                        mock_static_field_table_diff[j].type,
                        mock_static_field_table[j].type,
                        mock_static_field_table[j].type
                    );

                }

            }

        }

    }

}

static void battalion_dynamic_diff_table_init(
    unsigned int obj_count,
    BattalionDynamicTableDiff *mock_dynamic_table_diff,
    BattalionDynamicTable *mock_friendly_dynamic_table,
    BattalionDynamicTable *mock_enemy_dynamic_table,
    const ComplexFieldEntry *mock_dynamic_field_table_diff,
    const ComplexFieldEntry *mock_dynamic_field_table) {

    // this function assumes that the field tables are compatiable and counter parts of each other
    
    unsigned int diff_table_fields_count = complex_field_table_fields_count(mock_dynamic_field_table_diff);
    const char *keys[diff_table_fields_count];
    memset(keys, 0, sizeof(const char *) * diff_table_fields_count);
    for (unsigned int i = 0; i < diff_table_fields_count; i++) {

        keys[i] = mock_dynamic_field_table_diff[i].key;

    }

    unsigned int dynamic_table_fields_count = complex_field_table_fields_count(mock_dynamic_field_table);
    
    for (unsigned int o = 0; o < obj_count; o++) {

        for (unsigned int i = 0; i < diff_table_fields_count; i++) {

            for (unsigned int j = 0; j < dynamic_table_fields_count; j++) {

                if (keys[i] == mock_dynamic_field_table[j].key) {

                    if (strcmp(keys[i], "threat") == 0) {

                        enum ThreatLevel threat = UNKNOWN_THREAT;
                        unsigned int k = 0;
                        for (; k < dynamic_table_fields_count; k++) {
                        
                            if (strcmp(keys[k], "type") == 0) {

                                enum BattalionType friendly_type = (enum BattalionType)(*((*((char **)((char *)mock_friendly_dynamic_table + mock_dynamic_field_table[k].offset))) + mock_dynamic_field_table[k].size * o));
                                enum BattalionType enemy_type = (enum BattalionType)(*((*((char **)((char *)mock_enemy_dynamic_table + mock_dynamic_field_table[k].offset))) + mock_dynamic_field_table[k].size * o));
                        
                                threat = evaluate_battalion_threat(friendly_type, enemy_type);
                                break;

                            }

                        }

                        enum ThreatLevel *dest = (enum ThreatLevel *)((*((char **)((char *)mock_dynamic_table_diff + mock_dynamic_field_table_diff[k].offset))) + mock_dynamic_field_table_diff[k].size * o);
                        *dest = threat;

                        continue;

                    }

                    void *src_friendly_ptr = (void *)((*((char **)((char *)mock_friendly_dynamic_table + mock_dynamic_field_table[j].offset))) + mock_dynamic_field_table[j].size * o);
                    void *src_enemy_ptr = (void *)((*((char **)((char *)mock_enemy_dynamic_table + mock_dynamic_field_table[j].offset))) + mock_dynamic_field_table[j].size * o);
                    
                    unsigned long long int src_friendly = *((unsigned long long int *)src_friendly_ptr);
                    unsigned long long int src_enemy = *((unsigned long long int *)src_enemy_ptr);
                    
                    size_t size = mock_dynamic_field_table[j].size;
                    if (size < sizeof(unsigned long long int)) {

                        size_t shift = sizeof(unsigned long long int) - size;

                        src_friendly &= (-1ULL >> shift);
                        src_enemy &= (-1ULL >> shift);

                    }

                    void *dest = (void *)((*((char **)((char *)mock_dynamic_table_diff + mock_dynamic_field_table_diff[j].offset))) + mock_dynamic_field_table_diff[j].size * o);
                    subtract_type(
                        dest, &src_friendly, &src_enemy,
                        mock_dynamic_field_table_diff[j].type,
                        mock_dynamic_field_table[j].type,
                        mock_dynamic_field_table[j].type
                    );

                }

            }

        }

    }

}
