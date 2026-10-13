#include "battalion.h"
#include "battalion_internal.h"
#include "fatigue_types.h"
#include "field_entry_internal.h"
#include "health_pool.h"
#include "morale_types.h"
#include "position.h"
#include <math.h>
#include <stddef.h>
#include <stdbool.h>

#include "spreadsheet.h"
#include "spreadsheet_internal.h"

// private prototypes
static void set_battalion_formation(Battalion *battalion, unsigned int formation_width, unsigned int formation_height, unsigned int padding_between_units_x, unsigned int padding_between_units_y);

#define OFFSET_OF(type, member) offsetof(type, member)
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))

static const FieldEntry rally_basic_field_table[] = {

    { "timer",    OFFSET_OF(Rally, timer),    SIZE_OF(Rally, timer)    },
    { "rallying", OFFSET_OF(Rally, rallying), SIZE_OF(Rally, rallying) },
    { "obj_size", 0, sizeof(Rally) }

};
const FieldEntry *const rally_basic_field_table_ptr = rally_basic_field_table;

static const FieldEntry battalion_state_basic_field_table[] = {

    { "lifecycle", OFFSET_OF(BattalionState, lifecycle), SIZE_OF(BattalionState, lifecycle) },
    { "movement",  OFFSET_OF(BattalionState, movement),  SIZE_OF(BattalionState, movement)  },
    { "rally",     OFFSET_OF(BattalionState, rally),     SIZE_OF(BattalionState, rally)     },
    { "headless",  OFFSET_OF(BattalionState, headless),  SIZE_OF(BattalionState, headless)  },
    { "obj_size", 0, sizeof(BattalionState) }

};
const FieldEntry *const battalion_state_basic_field_table_ptr = battalion_state_basic_field_table;

static const FieldEntry battalion_statistics_basic_field_table[] = {

    { "unit_base_count",       OFFSET_OF(BattalionStatistics, unit_base_count),       SIZE_OF(BattalionStatistics, unit_base_count)       },
    { "unit_dead_count",       OFFSET_OF(BattalionStatistics, unit_dead_count),       SIZE_OF(BattalionStatistics, unit_dead_count)       },
    { "unit_withdrawed_count", OFFSET_OF(BattalionStatistics, unit_withdrawed_count), SIZE_OF(BattalionStatistics, unit_withdrawed_count) },
    { "obj_size", 0, sizeof(BattalionStatistics) }

};
const FieldEntry *const battalion_statistics_basic_field_table_ptr = battalion_statistics_basic_field_table;

static const ComplexFieldEntry rally_field_table[] = {

    { "timer",    OFFSET_OF(Rally, timer),    SIZE_OF(Rally, timer),    FLOAT, NULL },
    { "rallying", OFFSET_OF(Rally, rallying), SIZE_OF(Rally, rallying), BOOL,  NULL },
    { "obj_size", 0, sizeof(Rally), NOT_DEFINED, NULL }

};

static const ComplexFieldEntry battalion_state_field_table[] = {

    { "lifecycle", OFFSET_OF(BattalionState, lifecycle), SIZE_OF(BattalionState, lifecycle), S_INT,  NULL },
    { "movement",  OFFSET_OF(BattalionState, movement),  SIZE_OF(BattalionState, movement),  S_INT,  NULL },
    { "rally",     OFFSET_OF(BattalionState, rally),     SIZE_OF(BattalionState, rally),     STRUCT, rally_field_table },
    { "headless",  OFFSET_OF(BattalionState, headless),  SIZE_OF(BattalionState, headless),  BOOL,   NULL },
    { "obj_size", 0, sizeof(BattalionState), NOT_DEFINED, NULL }

};

static const ComplexFieldEntry battalion_statistics_field_table[] = {

    { "unit_base_count",       OFFSET_OF(BattalionStatistics, unit_base_count),       SIZE_OF(BattalionStatistics, unit_base_count),       U_INT, NULL },
    { "unit_dead_count",       OFFSET_OF(BattalionStatistics, unit_dead_count),       SIZE_OF(BattalionStatistics, unit_dead_count),       U_INT, NULL },
    { "unit_withdrawed_count", OFFSET_OF(BattalionStatistics, unit_withdrawed_count), SIZE_OF(BattalionStatistics, unit_withdrawed_count), U_INT, NULL },
    { "obj_size", 0, sizeof(BattalionStatistics), NOT_DEFINED, NULL }

};

static const ComplexFieldEntry battalion_field_table[] = {

    { "general",                 OFFSET_OF(Battalion, general),                 SIZE_OF(Battalion, general),                 NOT_DEFINED, NULL },
    { "unit",                    OFFSET_OF(Battalion, unit),                    SIZE_OF(Battalion, unit),                    NOT_DEFINED, NULL },
    { "state",                   OFFSET_OF(Battalion, state),                   SIZE_OF(Battalion, state),                   STRUCT,      battalion_state_field_table },
    { "morale",                  OFFSET_OF(Battalion, morale),                  SIZE_OF(Battalion, morale),                  STRUCT,      morale_field_table },
    { "fatigue",                 OFFSET_OF(Battalion, fatigue),                 SIZE_OF(Battalion, fatigue),                 STRUCT,      fatigue_field_table },
    { "health_pool",             OFFSET_OF(Battalion, health_pool),             SIZE_OF(Battalion, health_pool),             STRUCT,      health_pool_field_table },
    { "statistics",              OFFSET_OF(Battalion, statistics),              SIZE_OF(Battalion, statistics),              STRUCT,      battalion_statistics_field_table },
    { "is_active",               OFFSET_OF(Battalion, is_active),               SIZE_OF(Battalion, is_active),               BOOL,        NULL },
    { "pos",                     OFFSET_OF(Battalion, pos),                     SIZE_OF(Battalion, pos),                     STRUCT,      position_field_table },
    { "battalion_type",          OFFSET_OF(Battalion, battalion_type),          SIZE_OF(Battalion, battalion_type),          S_INT,       NULL },
    { "unit_count",              OFFSET_OF(Battalion, unit_count),              SIZE_OF(Battalion, unit_count),              U_INT,       NULL },
    { "padding_between_units_x", OFFSET_OF(Battalion, padding_between_units_x), SIZE_OF(Battalion, padding_between_units_x), U_INT,       NULL },
    { "padding_between_units_y", OFFSET_OF(Battalion, padding_between_units_y), SIZE_OF(Battalion, padding_between_units_y), U_INT,       NULL },
    { "unit_formation_width",    OFFSET_OF(Battalion, unit_formation_width),    SIZE_OF(Battalion, unit_formation_width),    U_INT,       NULL },
    { "unit_formation_height",   OFFSET_OF(Battalion, unit_formation_height),   SIZE_OF(Battalion, unit_formation_height),   U_INT,       NULL },
    { "unit_screen_width",       OFFSET_OF(Battalion, unit_screen_width),       SIZE_OF(Battalion, unit_screen_width),       FLOAT,       NULL },
    { "unit_screen_height",      OFFSET_OF(Battalion, unit_screen_height),      SIZE_OF(Battalion, unit_screen_height),      FLOAT,       NULL },
    { "R_Color",                 OFFSET_OF(Battalion, R_Color),                 SIZE_OF(Battalion, R_Color),                 U_CHAR,      NULL },
    { "G_Color",                 OFFSET_OF(Battalion, G_Color),                 SIZE_OF(Battalion, G_Color),                 U_CHAR,      NULL },
    { "B_Color",                 OFFSET_OF(Battalion, B_Color),                 SIZE_OF(Battalion, B_Color),                 U_CHAR,      NULL },
    { "Alpha",                   OFFSET_OF(Battalion, Alpha),                   SIZE_OF(Battalion, Alpha),                   U_CHAR,      NULL },
    { "speed",                   OFFSET_OF(Battalion, speed),                   SIZE_OF(Battalion, speed),                   FLOAT,       NULL },
    { "meele_attack",            OFFSET_OF(Battalion, meele_attack),            SIZE_OF(Battalion, meele_attack),            FLOAT,       NULL },
    { "meele_defense",           OFFSET_OF(Battalion, meele_defense),           SIZE_OF(Battalion, meele_defense),           FLOAT,       NULL },
    { "strength",                OFFSET_OF(Battalion, strength),                SIZE_OF(Battalion, strength),                FLOAT,       NULL },
    { "attack_speed",            OFFSET_OF(Battalion, attack_speed),            SIZE_OF(Battalion, attack_speed),            FLOAT,       NULL },
    { "range",                   OFFSET_OF(Battalion, range),                   SIZE_OF(Battalion, range),                   FLOAT,       NULL },
    { "charge_bonus",            OFFSET_OF(Battalion, charge_bonus),            SIZE_OF(Battalion, charge_bonus),            FLOAT,       NULL },
    { "armor",                   OFFSET_OF(Battalion, armor),                   SIZE_OF(Battalion, armor),                   FLOAT,       NULL },
    { "is_shielded",                OFFSET_OF(Battalion, is_shielded),                SIZE_OF(Battalion, is_shielded),                BOOL,        NULL },
    { "is_ranged",                  OFFSET_OF(Battalion, is_ranged),                  SIZE_OF(Battalion, is_ranged),                  BOOL,        NULL },
    { "is_large",                   OFFSET_OF(Battalion, is_large),                   SIZE_OF(Battalion, is_large),                   BOOL,        NULL },
    { "is_armor_piercing",          OFFSET_OF(Battalion, is_armor_piercing),          SIZE_OF(Battalion, is_armor_piercing),          BOOL,        NULL },
    { "is_anti_large",              OFFSET_OF(Battalion, is_anti_large),              SIZE_OF(Battalion, is_anti_large),              BOOL,        NULL },
    { "is_routing",                 OFFSET_OF(Battalion, is_routing),                 SIZE_OF(Battalion, is_routing),                 BOOL,        NULL },
    { "is_controllable",            OFFSET_OF(Battalion, is_controllable),            SIZE_OF(Battalion, is_controllable),            BOOL,        NULL },
    { "is_present",                 OFFSET_OF(Battalion, is_present),                 SIZE_OF(Battalion, is_present),                 BOOL,        NULL },
    { "unit_alive_count",        OFFSET_OF(Battalion, unit_alive_count),        SIZE_OF(Battalion, unit_alive_count),        U_INT,       NULL },
    { "start_entity_count",      OFFSET_OF(Battalion, start_entity_count),      SIZE_OF(Battalion, start_entity_count),      U_INT,       NULL },
    { "formation_x",             OFFSET_OF(Battalion, formation_x),             SIZE_OF(Battalion, formation_x),             U_CHAR,       NULL },
    { "formation_y",             OFFSET_OF(Battalion, formation_y),             SIZE_OF(Battalion, formation_y),             U_CHAR,       NULL },
    { "obj_size",                0,                                             sizeof(Battalion),                           NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const battalion_field_table_ptr = battalion_field_table;

#undef OFFSET_OF
#undef SIZE_OF

void init_battalion(Battalion *battalion, unsigned int battalion_size, enum BattalionType type) {

    BattalionSpreadsheet battalion_default = get_battalion_spreadsheet(type);

    unsigned int unit_alive_count = battalion_size;

    //battalion->general = NULL;
    //battalion->unit = NULL;
    battalion->state = (BattalionState){.lifecycle = NORMAL, .movement = STATIONARY, .rally = {.timer = 0, .rallying = false}, .headless = false};
    battalion->morale = (Morale){.state = EAGER, .current = 100, .base = 100, .modifiers = 0};
    battalion->fatigue = (Fatigue){.state = FRESH, .base = 100, .current = 100};
    battalion->health_pool = (HealthPool){.max_hp = battalion_default.hp, .current_hp = battalion_default.hp / unit_alive_count};
    battalion->statistics = (BattalionStatistics){.unit_base_count = battalion_size};
    battalion->is_active = 0;
    battalion->pos.x = 0;
    battalion->pos.y = 0;
    battalion->battalion_type = battalion_default.battalion_type;
    battalion->unit_count = unit_alive_count;

    battalion->unit_alive_count = unit_alive_count;
    battalion->formation_x = battalion_default.formation_x;
    battalion->formation_y = battalion_default.formation_y;
    battalion->attack_speed = battalion_default.attack_speed;
    battalion->sprite_type = battalion_default.sprite_type;

    battalion->padding_between_units_x = 0;
    battalion->padding_between_units_y = 0;
    battalion->unit_formation_width = 0;
    battalion->unit_formation_height = 0;
    battalion->unit_screen_width = 0;
    battalion->unit_screen_height = 0;
    battalion->R_Color = 0;
    battalion->G_Color = 0;
    battalion->B_Color = 0;
    battalion->Alpha = 0;
    battalion->speed = battalion_default.speed;
    battalion->meele_attack = battalion_default.meele_attack;
    battalion->meele_defense = battalion_default.meele_defense;
    battalion->strength = battalion_default.strength;
    battalion->range = battalion_default.range;
    battalion->charge_bonus = battalion_default.charge_bonus;
    battalion->armor = battalion_default.armor;
    battalion->is_shielded = battalion_default.is_shielded;
    battalion->is_ranged = battalion_default.is_ranged;
    battalion->is_large = battalion_default.is_large;
    battalion->is_armor_piercing = battalion_default.is_armor_piercing;
    battalion->is_anti_large = battalion_default.is_anti_large;
    battalion->is_routing = false;
    battalion->is_controllable = true;
    battalion->is_present = true;

    unsigned int formation_height = battalion_default.formation_x;
    unsigned int formation_width = ceilf(unit_alive_count / formation_height);
    unsigned int padding_between_units_x = 5;
    unsigned int padding_between_units_y = 5;

    set_battalion_formation(
        battalion,
        formation_width,
        formation_height,
        padding_between_units_x,
        padding_between_units_y
    );

}

static void set_battalion_formation(
    Battalion *battalion,
    unsigned int formation_width,
    unsigned int formation_height,
    unsigned int padding_between_units_x,
    unsigned int padding_between_units_y) {

    battalion->unit_formation_width = formation_width;
    battalion->unit_formation_height = formation_height;
    battalion->padding_between_units_x = padding_between_units_x;
    battalion->padding_between_units_y = padding_between_units_y;

}
