#include "general_table.h"
#include "general_table_internal.h"
#include "animation.h"
#include "animation_types.h"
#include "aos_soa_transformer.h"
#include "battlefield_types.h"
#include "camera.h"
#include "combat.h"
#include "direction_types.h"
#include "engine_internal.h"
#include "field_entry.h"
#include "formation.h"
#include "maps_internal.h"
#include "memory_arena.h"
#include "sprites_internal.h"
#include "spreadsheet.h"
#include "spreadsheet_internal.h"
#include "table_internal.h"
#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

// private prototypes
static void allocate_mock_soa_memory(
    unsigned int obj_count,
    GeneralStaticTable *mock_friendly_static_table,
    GeneralStaticTable *mock_enemy_static_table,
    //MockGeneralStaticTableDiff *mock_static_table_diff,
    GeneralDynamicTable *mock_friendly_dynamic_table,
    GeneralDynamicTable *mock_enemy_dynamic_table
    //MockGeneralDynamicTableDiff *mock_dynamic_table_diff
    );
static void allocate_mem_for_mock(unsigned int obj_count, const ComplexFieldEntry *field_table, void *mock);
static void general_static_table_init(
    General *general,
    GeneralStaticTable *mock_static_table,
    const ComplexFieldEntry *general_complex_field_table,
    const ComplexFieldEntry *mock_static_field_table,
    unsigned int battalion_count);
static void general_dynamic_table_init(
    General *general,
    GeneralDynamicTable *mock_dynamic_table,
    const ComplexFieldEntry *general_complex_field_table,
    const ComplexFieldEntry *mock_dynamic_field_table,
    unsigned int battalion_count);
static void update_stats(Table *tables, unsigned int i);
static void update_status(Table *tables, Map *map, unsigned int i);
static void try_engage_enemy(Table *tables, unsigned int battalion_count, unsigned int battalion_size, unsigned int i);
static void update_position(Table *tables, enum ArmyBattlefieldSide side, unsigned int i);
static void update_direction(Table *tables, unsigned int i);
static void resolve_attack(Table *tables, unsigned int i, unsigned int battalion_size, float dt);
static void animation_table_update(Table *tables, unsigned int i);
static float direction_from_vec(float vec_x, float vec_y, float fallback);
static float wrap_direction(float dir);

static void *general_table_start_address_alloc = NULL;

#define OFFSET_OF(type, member) offsetof(type, member)
#define ELEMENT_SIZE_OF(type, member) (sizeof(*(((type *)0)->member)))

static const ComplexFieldEntry general_static_table_field_table[] = {

    { "base_meele_attack",  OFFSET_OF(GeneralStaticTable, base_meele_attack),  ELEMENT_SIZE_OF(GeneralStaticTable, base_meele_attack),  FLOAT, NULL },
    { "base_meele_defense", OFFSET_OF(GeneralStaticTable, base_meele_defense), ELEMENT_SIZE_OF(GeneralStaticTable, base_meele_defense), FLOAT, NULL },
    { "base_strength",      OFFSET_OF(GeneralStaticTable, base_strength),      ELEMENT_SIZE_OF(GeneralStaticTable, base_strength),      FLOAT, NULL },
    { "range",              OFFSET_OF(GeneralStaticTable, range),              ELEMENT_SIZE_OF(GeneralStaticTable, range),              FLOAT, NULL },
    { "charge_bonus",       OFFSET_OF(GeneralStaticTable, charge_bonus),       ELEMENT_SIZE_OF(GeneralStaticTable, charge_bonus),       FLOAT, NULL },
    { "armor",              OFFSET_OF(GeneralStaticTable, armor),              ELEMENT_SIZE_OF(GeneralStaticTable, armor),              FLOAT, NULL },
    { "attack_speed",       OFFSET_OF(GeneralStaticTable, attack_speed),       ELEMENT_SIZE_OF(GeneralStaticTable, attack_speed),       FLOAT, NULL },
    { "sprite_type",  OFFSET_OF(GeneralStaticTable, sprite_type),  ELEMENT_SIZE_OF(GeneralStaticTable, sprite_type),  S_INT, NULL },
    { "is_shielded",           OFFSET_OF(GeneralStaticTable, is_shielded),           ELEMENT_SIZE_OF(GeneralStaticTable, is_shielded),           BOOL,  NULL },
    { "is_ranged",             OFFSET_OF(GeneralStaticTable, is_ranged),             ELEMENT_SIZE_OF(GeneralStaticTable, is_ranged),             BOOL,  NULL },
    { "is_large",              OFFSET_OF(GeneralStaticTable, is_large),              ELEMENT_SIZE_OF(GeneralStaticTable, is_large),              BOOL,  NULL },
    { "is_armor_piercing",     OFFSET_OF(GeneralStaticTable, is_armor_piercing),     ELEMENT_SIZE_OF(GeneralStaticTable, is_armor_piercing),     BOOL,  NULL },
    { "is_anti_large",         OFFSET_OF(GeneralStaticTable, is_anti_large),         ELEMENT_SIZE_OF(GeneralStaticTable, is_anti_large),         BOOL,  NULL },
    { "obj_size", 0, sizeof(GeneralStaticTable), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const general_static_table_field_table_ptr = general_static_table_field_table;

static const ComplexFieldEntry general_dynamic_table_field_table[] = {

    { "direction",           OFFSET_OF(GeneralDynamicTable, direction),           ELEMENT_SIZE_OF(GeneralDynamicTable, direction),           S_INT,  NULL },
    { "target_type",         OFFSET_OF(GeneralDynamicTable, target_type),         ELEMENT_SIZE_OF(GeneralDynamicTable, target_type),         S_INT,  NULL },
    { "personal_target_idx", OFFSET_OF(GeneralDynamicTable, personal_target_idx), ELEMENT_SIZE_OF(GeneralDynamicTable, personal_target_idx), U_INT,  NULL },
    { "animation",           OFFSET_OF(GeneralDynamicTable, animation),           ELEMENT_SIZE_OF(GeneralDynamicTable, animation),           S_INT,  NULL },
    { "current_frame",       OFFSET_OF(GeneralDynamicTable, current_frame),       ELEMENT_SIZE_OF(GeneralDynamicTable, current_frame),       U_CHAR, NULL },
    { "order_x",             OFFSET_OF(GeneralDynamicTable, order_x),             ELEMENT_SIZE_OF(GeneralDynamicTable, order_x),             FLOAT,  NULL },
    { "order_y",             OFFSET_OF(GeneralDynamicTable, order_y),             ELEMENT_SIZE_OF(GeneralDynamicTable, order_y),             FLOAT,  NULL },
    { "vec_x",               OFFSET_OF(GeneralDynamicTable, vec_x),               ELEMENT_SIZE_OF(GeneralDynamicTable, vec_x),               FLOAT,  NULL },
    { "vec_y",               OFFSET_OF(GeneralDynamicTable, vec_y),               ELEMENT_SIZE_OF(GeneralDynamicTable, vec_y),               FLOAT,  NULL },
    { "x",                   OFFSET_OF(GeneralDynamicTable, x),                   ELEMENT_SIZE_OF(GeneralDynamicTable, x),                   FLOAT,  NULL },
    { "y",                   OFFSET_OF(GeneralDynamicTable, y),                   ELEMENT_SIZE_OF(GeneralDynamicTable, y),                   FLOAT,  NULL },
    { "hp",                  OFFSET_OF(GeneralDynamicTable, hp),                  ELEMENT_SIZE_OF(GeneralDynamicTable, hp),                  FLOAT,  NULL },
    { "meele_attack",        OFFSET_OF(GeneralDynamicTable, meele_attack),        ELEMENT_SIZE_OF(GeneralDynamicTable, meele_attack),        FLOAT,  NULL },
    { "meele_defense",       OFFSET_OF(GeneralDynamicTable, meele_defense),       ELEMENT_SIZE_OF(GeneralDynamicTable, meele_defense),       FLOAT,  NULL },
    { "strength",            OFFSET_OF(GeneralDynamicTable, strength),            ELEMENT_SIZE_OF(GeneralDynamicTable, strength),            FLOAT,  NULL },
    { "attack_timer",        OFFSET_OF(GeneralDynamicTable, attack_timer),        ELEMENT_SIZE_OF(GeneralDynamicTable, attack_timer),        FLOAT,  NULL },
    { "is_target_in_range",  OFFSET_OF(GeneralDynamicTable, is_target_in_range),  ELEMENT_SIZE_OF(GeneralDynamicTable, is_target_in_range),  BOOL,   NULL },
    { "is_present",          OFFSET_OF(GeneralDynamicTable, is_present),          ELEMENT_SIZE_OF(GeneralDynamicTable, is_present),          BOOL,   NULL },
    { "is_engaged",          OFFSET_OF(GeneralDynamicTable, is_engaged),          ELEMENT_SIZE_OF(GeneralDynamicTable, is_engaged),          BOOL,   NULL },
    { "is_engaging",         OFFSET_OF(GeneralDynamicTable, is_engaging),         ELEMENT_SIZE_OF(GeneralDynamicTable, is_engaging),         BOOL,   NULL },
    { "obj_size", 0, sizeof(GeneralDynamicTable), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const general_dynamic_table_field_table_ptr = general_dynamic_table_field_table;

#undef OFFSET_OF
#undef ELEMENT_SIZE_OF

void general_table_init(Table *player_tables, Table *enemy_tables, General *player_general, General *enemy_general, unsigned int battalion_count) {

    GeneralStaticTable mock_friendly_static_table = {0};
    GeneralStaticTable mock_enemy_static_table = {0};
    //MockGeneralStaticTableDiff mock_friendly_static_table_diff = {0};
    //MockGeneralStaticTableDiff mock_enemy_static_table_diff = {0};
    GeneralDynamicTable mock_friendly_dynamic_table = {0};
    GeneralDynamicTable mock_enemy_dynamic_table = {0};
    //MockGeneralDynamicTableDiff mock_friendly_dynamic_table_diff = {0};
    //MockGeneralDynamicTableDiff mock_enemy_dynamic_table_diff = {0};

    battalion_count /= 2; // half total battalion count as each side always has the same battalion count

    allocate_mock_soa_memory(
        battalion_count,
        &mock_friendly_static_table,
        &mock_enemy_static_table,
        //&mock_static_table_diff,
        &mock_friendly_dynamic_table,
        &mock_enemy_dynamic_table);
        //&mock_dynamic_table_diff

    general_static_table_init(player_general, &mock_friendly_static_table, general_complex_field_table_ptr, general_static_table_field_table_ptr, battalion_count);
    general_static_table_init(enemy_general, &mock_enemy_static_table, general_complex_field_table_ptr, general_static_table_field_table_ptr, battalion_count);
    general_dynamic_table_init(player_general, &mock_friendly_dynamic_table, general_complex_field_table_ptr, general_dynamic_table_field_table_ptr, battalion_count);
    general_dynamic_table_init(enemy_general, &mock_enemy_dynamic_table, general_complex_field_table_ptr, general_dynamic_table_field_table_ptr, battalion_count);
    //battalion_static_diff_table_init(battalion_count, mock_friendly_static_table_diff, mock_friendly_static_table, mock_enemy_static_table, mock_static_field_table_diff, mock_static_field_table);
    //battalion_dynamic_diff_table_init(battalion_count, mock_friendly_dynamic_table_diff, mock_friendly_dynamic_table, mock_enemy_dynamic_table, mock_dynamic_field_table_diff, mock_dynamic_field_table);

    //battalion_static_diff_table_init(battalion_count, mock_enemy_static_table_diff, mock_enemy_static_table, mock_friendly_static_table, mock_static_field_table_diff, mock_static_field_table);
    //battalion_dynamic_diff_table_init(battalion_count, mock_enemy_dynamic_table_diff, mock_enemy_dynamic_table, mock_friendly_dynamic_table, mock_dynamic_field_table_diff, mock_dynamic_field_table);

    /*GeneralComparisonTable general_tables = {
        .friendly_static = (GeneralStaticTable)mock_friendly_static_table,
        .enemy_static = (GeneralStaticTable)mock_enemy_static_table,
        //.friendly_static_diff = (GeneralStaticTableDiff)mock_friendly_static_table_diff;
        //.enemy_static_diff = (GeneralStaticTableDiff)mock_enemy_static_table_diff;
        .friendly_dynamic = (GeneralDynamicTable)mock_friendly_dynamic_table,
        .enemy_dynamic = (GeneralDynamicTable)mock_enemy_dynamic_table
        //.friendly_dynamic_diff = (GeneralDynamicTableDiff)mock_friendly_dynamic_table_diff;
        //.enemy_dynamic_diff = (GeneralDynamicTableDiff)mock_enemy_dynamic_table_diff;
    };*/

    player_tables->general_tables.friendly_static = mock_friendly_static_table;
    player_tables->general_tables.enemy_static = mock_enemy_static_table;
    enemy_tables->general_tables.friendly_static = mock_enemy_static_table;
    enemy_tables->general_tables.enemy_static = mock_friendly_static_table;

    player_tables->general_tables.friendly_dynamic = mock_friendly_dynamic_table;
    player_tables->general_tables.enemy_dynamic = mock_enemy_dynamic_table;
    enemy_tables->general_tables.friendly_dynamic = mock_enemy_dynamic_table;
    enemy_tables->general_tables.enemy_dynamic = mock_friendly_dynamic_table;

}

void general_table_update(Table *tables, Map *map, enum ArmyBattlefieldSide side, unsigned int battalion_count, unsigned int battalion_size, float dt) {

    for (unsigned int i = 0; i < battalion_count; i++) {

        _Bool is_present = tables->general_tables.friendly_dynamic.is_present[i];
        if (!is_present) continue;

        update_stats(tables, i);
        update_status(tables, map, i);
        try_engage_enemy(tables, battalion_count, battalion_size, i);
        update_position(tables, side, i);
        update_direction(tables, i);
        resolve_attack(tables, i, battalion_size, dt);
        animation_table_update(tables, i);

    }

}

void general_table_render(Table *tables, unsigned int i) {

    const enum Direction direction = tables->general_tables.friendly_dynamic.direction[i];
    const float x = tables->general_tables.friendly_dynamic.x[i];
    const float y = tables->general_tables.friendly_dynamic.y[i];
    //const enum BattalionType battalion_type = tables->battalion_tables.friendly_static.battalion_type[i];
    //enum BattalionState battalion_state = tables->battalion_tables.friendly_static.state[i];
    const unsigned char current_frame = tables->general_tables.friendly_dynamic.current_frame[i];
    const enum Animation anim_type = tables->general_tables.friendly_dynamic.animation[i];
    //const enum Sprites sprite_type = get_battalion_spreadsheet(battalion_type).sprite_type;
    const enum Sprites sprite_type = tables->general_tables.friendly_static.sprite_type[i];
    const Sprite *sprite = &engine.sprite_pack->sprite[sprite_type][anim_type];
    signed int png_width = sprite->width;
    signed int png_height = sprite->height;

    signed int sprite_frame_width = png_width / sprite->frames_count;
    signed int sprite_frame_height = png_height;

    SDL_Rect sprite_slice = {
        (sprite_frame_width * current_frame),
        0,
        sprite_frame_width,
        sprite_frame_height
    };

    SDL_Rect sprite_position = {
        (signed int)x,
        (signed int)y,
        (signed int)sprite_frame_width,
        (signed int)sprite_frame_height
    };
    
    camera_world_to_screen(&sprite_position);

    const SDL_Rect *rect1 = &sprite_slice;
    const SDL_Rect *rect2 = &sprite_position;

    SDL_Renderer *renderer = engine.renderer;
    SDL_Texture *texture = sprite->texture;
    if (direction == LEFT) {
    
        SDL_RenderCopy(renderer, texture, rect1, rect2);

    } else if (direction == RIGHT) {

        SDL_RenderCopyEx(renderer, texture, rect1, rect2, 0, NULL, SDL_FLIP_HORIZONTAL);

    }

}

void general_table_destroy(void) {

    memory_arena_reset_to_address(general_table_start_address_alloc);

}

static void update_stats(Table *tables, unsigned int i) {

    enum MoraleState morale_state = tables->battalion_tables.friendly_dynamic.morale_state[i];
    enum FatigueState fatigue_state = tables->battalion_tables.friendly_dynamic.fatigue_state[i];
    float base_meele_attack = tables->general_tables.friendly_static.base_meele_attack[i];
    float base_meele_defense = tables->general_tables.friendly_static.base_meele_defense[i];
    float base_strength = tables->general_tables.friendly_static.base_strength[i];
    float meele_attack = tables->general_tables.friendly_dynamic.meele_attack[i];
    float meele_defense = tables->general_tables.friendly_dynamic.meele_defense[i];
    float strength = tables->general_tables.friendly_dynamic.strength[i];

    meele_attack = base_meele_attack - (morale_state * 2 + fatigue_state * 3);
    meele_defense = base_meele_defense - (morale_state * 2 + fatigue_state * 3);
    strength = base_strength - (fatigue_state * 3);

    tables->general_tables.friendly_dynamic.meele_attack[i] = meele_attack;
    tables->general_tables.friendly_dynamic.meele_defense[i] = meele_defense;
    tables->general_tables.friendly_dynamic.strength[i] = strength;

}

static void update_status(Table *tables, Map *map, unsigned int i) {

    // is is_present
    const float hp = tables->general_tables.friendly_dynamic.hp[i];
    unsigned int dead_entity_count = tables->battalion_tables.friendly_dynamic.dead_entity_count[i];
    unsigned int withdrawed_entity_count = tables->battalion_tables.friendly_dynamic.withdrawed_entity_count[i];
    const float x = tables->battalion_tables.friendly_dynamic.x[i];
    const float y = tables->battalion_tables.friendly_dynamic.y[i];
    const _Bool is_dead = hp <= 0.0f;
    const _Bool withdrawed = (x < map->mapLeftLimit) | (x > map->mapRightLimit) | (y < map->mapTopLimit) | (y > map->mapBottomLimit);
    dead_entity_count += is_dead;
    withdrawed_entity_count += withdrawed;
    const _Bool is_present = !(is_dead | withdrawed);

    tables->battalion_tables.friendly_dynamic.dead_entity_count[i] = dead_entity_count;
    tables->battalion_tables.friendly_dynamic.withdrawed_entity_count[i] = withdrawed_entity_count;
    tables->general_tables.friendly_dynamic.is_present[i] = is_present;

    // is is_engaged
    // is is_engaging
    _Bool is_target_in_range = tables->general_tables.friendly_dynamic.is_target_in_range[i];
    enum TargetType personal_target_type = tables->general_tables.friendly_dynamic.target_type[i];
    enum OrderType order_type = tables->battalion_order.type[i];

    const _Bool is_engaged = is_target_in_range & (order_type == ORDER_ATTACK) & (personal_target_type != UNKNOWN_TARGET);
    tables->general_tables.friendly_dynamic.is_engaged[i] = is_engaged;

    const _Bool is_engaging = !is_target_in_range & (order_type == ORDER_ATTACK) & (personal_target_type != UNKNOWN_TARGET);
    tables->general_tables.friendly_dynamic.is_engaging[i] = is_engaging;

}

static void try_engage_enemy(Table *tables, unsigned int battalion_count, unsigned int battalion_size, unsigned int i) {

    if (!tables->battalion_tables.friendly_dynamic.is_engaging[i] ||
        !tables->battalion_tables.friendly_dynamic.is_engaged) return;
    if (tables->general_tables.friendly_dynamic.is_engaged[i]) return;

    const float x = tables->general_tables.friendly_dynamic.x[i];
    const float y = tables->general_tables.friendly_dynamic.y[i];

    _Bool is_engaged = false;
    unsigned int closest_enemy_idx = -1U;
    enum TargetType personal_target_type = UNKNOWN_TARGET;
    #define MEELE_SEARCH_RANGE 300.0f
    float shortest_dist = MEELE_SEARCH_RANGE * MEELE_SEARCH_RANGE;
    unsigned int enemy_battalion_count = battalion_count / 2;
    for (unsigned int j = 0; j < enemy_battalion_count; j++) {

        if (!tables->battalion_tables.enemy_dynamic.is_present[j]) continue;

        float enemy_x = tables->battalion_tables.enemy_dynamic.x[j];
        float enemy_y = tables->battalion_tables.enemy_dynamic.y[j];

        float dx = enemy_x - x;
        float dy = enemy_y - y;
        float dist = dx * dx + dy * dy;
        if (dist > MEELE_SEARCH_RANGE * MEELE_SEARCH_RANGE) continue;
        #undef MEELE_SEARCH_RANGE
        if (tables->general_tables.enemy_dynamic.is_present[j]) {

            enemy_x = tables->general_tables.enemy_dynamic.x[j];
            enemy_y = tables->general_tables.enemy_dynamic.y[j];
            
            dx = enemy_x - x;
            dy = enemy_y - y;
            dist = dx * dx + dy * dy;

            if (dist < shortest_dist) {

                closest_enemy_idx = j;
                shortest_dist = dist;
                personal_target_type = TARGET_GENERAL;
                
            }

        }

        for (unsigned int idx = battalion_size * j, k = 0; k < battalion_size; k++) {

            idx += k;
            if (!tables->unit_tables.enemy_dynamic.is_present[idx]) continue;
            
            enemy_x = tables->unit_tables.enemy_dynamic.x[idx];
            enemy_y = tables->unit_tables.enemy_dynamic.y[idx];
            
            dx = enemy_x - x;
            dy = enemy_y - y;
            dist = dx * dx + dy * dy;

            if (dist < shortest_dist) {

                closest_enemy_idx = idx;
                shortest_dist = dist;
                personal_target_type = TARGET_UNIT;

            }

        }

    }

    is_engaged = (personal_target_type != UNKNOWN_TARGET) & (closest_enemy_idx != -1U);
    tables->general_tables.friendly_dynamic.is_engaged[i] = is_engaged;
    tables->general_tables.friendly_dynamic.target_type[i] = personal_target_type;
    tables->general_tables.friendly_dynamic.personal_target_idx[i] = closest_enemy_idx;

}

static void update_position(Table *tables, enum ArmyBattlefieldSide side, unsigned int i) {

    float x = tables->general_tables.friendly_dynamic.x[i];
    float y = tables->general_tables.friendly_dynamic.y[i];
    float vec_x = tables->general_tables.friendly_dynamic.vec_x[i];
    float vec_y = tables->general_tables.friendly_dynamic.vec_y[i];
    float speed = tables->battalion_tables.friendly_dynamic.speed[i];

    if (tables->battalion_tables.friendly_dynamic.lifecycle_state[i] == ROUTING) {

        if (side == ARMY_SIDE_LEFT) {

            vec_x = -speed * 0.01f;

        } else {

            vec_x = speed * 0.01f;

        }

    } else if (tables->battalion_tables.friendly_dynamic.is_engaging[i]) {

        //unsigned int target_battalion_idx = tables->battalion_tables.friendly_dynamic.target_battalion_idx[i];
        //float target_battalion_x = tables->battalion_tables.enemy_dynamic.x[target_battalion_idx];
        //float target_battalion_y = tables->battalion_tables.enemy_dynamic.y[target_battalion_idx];
        
        float target_x = tables->general_tables.friendly_dynamic.order_x[i];
        float target_y = tables->general_tables.friendly_dynamic.order_y[i];

        float bdx = target_x - x;
        float bdy = target_y - y;
        float bdist = bdx * bdx + bdy * bdy;

        float blen = sqrtf(bdist);
        float bk = (speed * 0.01f) / blen;

        vec_x += bdx * bk;
        vec_y += bdy * bk;

        #define ENTITY_CAN_START_ENGAGING_RANGE 200.0f
        if (blen > ENTITY_CAN_START_ENGAGING_RANGE) goto POSITION_UPDATE;
        #undef ENTITY_CAN_START_ENGAGING_RANGE
        if (!tables->general_tables.friendly_dynamic.is_engaging[i]) {

            enum TargetType personal_target_type = tables->general_tables.friendly_dynamic.target_type[i];
            unsigned int personal_target_idx = tables->general_tables.friendly_dynamic.personal_target_idx[i];
            float enemy_x = 0;
            float enemy_y = 0;
            if (personal_target_type == TARGET_UNIT) {

                enemy_x = tables->unit_tables.enemy_dynamic.x[personal_target_idx];
                enemy_y = tables->unit_tables.enemy_dynamic.y[personal_target_idx];

            } else if (personal_target_type == TARGET_GENERAL) {

                enemy_x = tables->general_tables.enemy_dynamic.x[personal_target_idx];
                enemy_y = tables->general_tables.enemy_dynamic.y[personal_target_idx];

            }

            float dx = enemy_x - x;
            float dy = enemy_y - y;
            float dist = dx * dx + dy * dy;
            float range = tables->general_tables.friendly_static.range[i];
            if (dist > range * range) {

                float len = sqrtf(dist);
                float k = (speed * 0.01f) / len;

                vec_x += dx * k;
                vec_y += dy * k;

                float ndx = dx - vec_x;
                float ndy = dy - vec_y;
                float new_dist = ndx * ndx + ndy * ndy;
                tables->general_tables.friendly_dynamic.is_target_in_range[i] = new_dist <= range * range;

            }

        }
    
    }

    POSITION_UPDATE:
    x += vec_x;
    y += vec_y;

    tables->general_tables.friendly_dynamic.vec_x[i] = 0.0f;
    tables->general_tables.friendly_dynamic.vec_y[i] = 0.0f;
    tables->general_tables.friendly_dynamic.x[i] = x;
    tables->general_tables.friendly_dynamic.y[i] = y;

}

/*
static void update_position(Table *tables, Map map, enum ArmyBattlefieldSide side,unsigned int i, unsigned int battalion_size) {

    float x = tables->general_tables.friendly_dynamic.x[i];
    float y = tables->general_tables.friendly_dynamic.y[i];
    float vec_x = tables->general_tables.friendly_dynamic.vec_x[i];
    float vec_y = tables->general_tables.friendly_dynamic.vec_y[i];
    float speed = tables->battalion_tables.friendly_dynamic.speed[i];

    if (tables->battalion_tables.friendly_dynamic.lifecycle[i] == ROUTING) {

        if (side == ARMY_SIDE_LEFT) {

            vec_x = -speed * 0.01f;

        } else {

            vec_x = speed * 0.01f;

        }

    } else if (tables->battalion_tables.friendly_dynamic.is_engaged[i]) {

        if (!tables->general_tables.friendly_dynamic.is_engaging[i]) {

            enum TargetType personal_target_type = tables->general_tables.friendly_dynamic.personal_target_type[i];
            unsigned int personal_target_idx = tables->general_tables.friendly_dynamic.personal_target_idx[i];
            float enemy_x = 0;
            float enemy_y = 0;
            if (personal_target_type == UNIT) {

                enemy_x = tables->unit_tables.enemy_dynamic.x[personal_target_idx];
                enemy_y = tables->unit_tables.enemy_dynamic.y[personal_target_idx];

            } else if (personal_target_type == GENERAL) {

                enemy_x = tables->general_tables.enemy_dynamic.x[personal_target_idx];
                enemy_y = tables->general_tables.enemy_dynamic.y[personal_target_idx];

            }

            float dx = enemy_x - x;
            float dy = enemy_y - y;
            float dist = dx * dx + dy * dy;
            float range = tables->general_tables.friendly_static.range[i];
            if (dist > range * range) {

                float len = sqrtf(dist);
                float k = (speed * 0.01f) / len;

                vec_x += dx * k;
                vec_y += dy * k;

                float ndx = dx - vec_x;
                float ndy = dy - vec_y;
                float new_dist = ndx * ndx + ndy * ndy;
                tables->general_tables.friendly_dynamic.is_target_in_range[i] = new_dist <= range * range;

            }

        }
    
    }

    x += vec_x;
    y += vec_y;

    tables->general_tables.friendly_dynamic.vec_x[i] = 0.0f;
    tables->general_tables.friendly_dynamic.vec_y[i] = 0.0f;
    tables->general_tables.friendly_dynamic.x[i] = x;
    tables->general_tables.friendly_dynamic.y[i] = y;

}
*/

static void update_direction(Table *tables, unsigned int i) {

    float vx = tables->general_tables.friendly_dynamic.vec_x[i];
    float vy = tables->general_tables.friendly_dynamic.vec_y[i];
    enum Direction direction = tables->general_tables.friendly_dynamic.direction[i];
    float dir = direction_from_vec(vx, vy, direction == LEFT ? 2.0f : 0.0f);
    direction = (dir < 2.0f) ? RIGHT : LEFT;
    tables->general_tables.friendly_dynamic.direction[i] = direction;

}

#define PI 3.14159274f
#define DIR_STEPS 4.0f
static float direction_from_vec(float vec_x, float vec_y, float fallback) {
    if (vec_x * vec_x + vec_y * vec_y < 0.0001f) return fallback;

    float angle = atan2f(vec_y, vec_x);
    return wrap_direction(angle * (DIR_STEPS / (2.0f * PI)));
}

static float wrap_direction(float dir) {
    dir = fmodf(dir, DIR_STEPS);
    if (dir < 0.0f) dir += DIR_STEPS;
    return dir;
}
#undef PI
#undef DIR_STEPS

static void resolve_attack(Table *tables, unsigned int i, unsigned int battalion_size, float dt) {

    if (!tables->general_tables.friendly_dynamic.is_engaged[i]) return;
    float attack_timer = tables->general_tables.friendly_dynamic.attack_timer[i];
    attack_timer -= dt;
    if (attack_timer > 0.0f) return;

    float attack_speed = tables->general_tables.friendly_static.attack_speed[i];
    tables->general_tables.friendly_dynamic.attack_timer[i] = attack_speed;

    float strength = tables->general_tables.friendly_dynamic.strength[i];
    float charge_bonus = tables->general_tables.friendly_static.charge_bonus[i];
    float meele_attack = tables->general_tables.friendly_dynamic.meele_attack[i];
    unsigned int personal_target_idx = tables->general_tables.friendly_dynamic.personal_target_idx[i];
    enum TargetType personal_target_type = tables->general_tables.friendly_dynamic.target_type[i];
    _Bool is_armor_piercing = tables->general_tables.friendly_static.is_armor_piercing[i];
    float *target_hp = NULL;
    float target_meele_defense = 0.0f;
    float target_armor = 0.0f;
    _Bool is_target_large = 0;
    
    if (personal_target_type == TARGET_UNIT) {

        target_hp = &tables->unit_tables.enemy_dynamic.hp[personal_target_idx];
        target_meele_defense = tables->battalion_tables.enemy_dynamic.meele_defense[personal_target_idx / battalion_size];
        target_armor = tables->battalion_tables.enemy_static.armor[personal_target_idx / battalion_size];
        is_target_large = tables->battalion_tables.enemy_static.is_large[personal_target_idx / battalion_size];

    } else if (personal_target_type == TARGET_GENERAL) {

        target_hp = &tables->general_tables.enemy_dynamic.hp[personal_target_idx];
        target_meele_defense = tables->general_tables.enemy_dynamic.meele_defense[personal_target_idx];
        target_armor = tables->general_tables.enemy_static.armor[personal_target_idx];
        is_target_large = tables->general_tables.enemy_static.is_large[personal_target_idx];

    }

    if (*target_hp <= 0.0f) {

        tables->general_tables.friendly_dynamic.is_engaged[i] = false;
        tables->general_tables.friendly_dynamic.personal_target_idx[i] = -1U;
        tables->general_tables.friendly_dynamic.target_type[i] = UNKNOWN_TARGET;
        return;
        
    }

    _Bool is_charging = tables->battalion_tables.friendly_dynamic.is_charging[i];
    float damage = strength + charge_bonus * is_charging;
    damage *= (is_target_large) ? 1.5f : 1.0f;
    target_armor = (is_armor_piercing) ? target_armor / 2 : target_armor;
    damage = apply_armor(damage, target_armor);
    float chance = hit_chance(meele_attack, target_meele_defense);
    _Bool hit = roll_hit(chance);

    if (hit) *target_hp -= damage;

}

static void animation_table_update(Table *tables, unsigned int i) {

    _Bool advance_frame = animation_tick();
    if (!advance_frame) return;

    //const enum BattalionType battalion_type = tables->battalion_tables.friendly_static.battalion_type[i];
    const enum BattalionMovementMode movement_state = tables->battalion_tables.friendly_dynamic.movement_state[i];
    const enum BattalionLifecycle lifecycle_state = tables->battalion_tables.friendly_dynamic.lifecycle_state[i];
    const _Bool is_engaged = tables->general_tables.friendly_dynamic.is_engaged[i];
    const _Bool is_target_in_range = tables->general_tables.friendly_dynamic.is_target_in_range[i];
    unsigned char current_frame = tables->general_tables.friendly_dynamic.current_frame[i];
    //const enum Sprites sprite_type = get_battalion_spreadsheet(battalion_type).sprite_type;
    const enum Sprites sprite_type = tables->general_tables.friendly_static.sprite_type[i];
    enum Animation anim = tables->general_tables.friendly_dynamic.animation[i];
    const Sprite *sprite = &engine.sprite_pack->sprite[sprite_type][anim];
    unsigned int frames_count = sprite->frames_count;

    enum Animation new_animation = -1;
    switch (movement_state) {

        case STATIONARY:
            new_animation = IDLE;
            break;
        case WALKING:
            new_animation = RUN;
            break;
        case RUNNING:
            new_animation = RUN;
            break;
        case CHARGING:
            new_animation = RUN;
            break;
        case FIGHTING:
            new_animation = ATTACK;
            break;
        default:
            break;

    }

    if (lifecycle_state == ROUTING) new_animation = RUN;
    if (is_engaged) new_animation = RUN;
    if (is_target_in_range) new_animation = ATTACK;

    if (frames_count == 0) {
        fprintf(stderr, "Frame count is 0.\n");
        exit(EXIT_FAILURE);
    }

    current_frame =
        (current_frame == frames_count - 1) ?
        0 : current_frame + 1;

    if (anim != new_animation) {
        anim = new_animation;
        current_frame = 0;
        frames_count = engine.sprite_pack->sprite[sprite_type][new_animation].frames_count;
    }

    tables->general_tables.friendly_dynamic.animation[i] = anim;
    tables->general_tables.friendly_dynamic.current_frame[i] = current_frame;

}

static void allocate_mock_soa_memory(
    unsigned int obj_count,
    GeneralStaticTable *mock_friendly_static_table,
    GeneralStaticTable *mock_enemy_static_table,
    //MockGeneralStaticTableDiff *mock_static_table_diff,
    GeneralDynamicTable *mock_friendly_dynamic_table,
    GeneralDynamicTable *mock_enemy_dynamic_table
    //MockGeneralDynamicTableDiff *mock_dynamic_table_diff
    ) {

    general_table_start_address_alloc = memory_arena_current_pointer();
    allocate_mem_for_mock(obj_count, general_static_table_field_table_ptr, mock_friendly_static_table);
    allocate_mem_for_mock(obj_count, general_static_table_field_table_ptr, mock_enemy_static_table);
    //allocate_mem_for_mock(obj_count, general_static_table_diff_field_table_ptr, mock_static_table_diff);
    allocate_mem_for_mock(obj_count, general_dynamic_table_field_table_ptr, mock_friendly_dynamic_table);
    allocate_mem_for_mock(obj_count, general_dynamic_table_field_table_ptr, mock_enemy_dynamic_table);
    //allocate_mem_for_mock(obj_count, general_dynamic_table_diff_field_table_ptr, mock_dynamic_table_diff);

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

static void general_static_table_init(
    General *general,
    GeneralStaticTable *mock_static_table,
    const ComplexFieldEntry *general_complex_field_table,
    const ComplexFieldEntry *mock_static_field_table,
    unsigned int battalion_count) {

    aos_soa_transform(general, (void **)mock_static_table, general_complex_field_table, mock_static_field_table, battalion_count, 13,
    "sprite_type", "base_meele_attack", "base_meele_defense", "base_strength", "range", "charge_bonus", "armor", "attack_speed", "is_shielded", "is_ranged", "is_large", "is_armor_piercing", "is_anti_large");

}

static void general_dynamic_table_init(
    General *general,
    GeneralDynamicTable *mock_dynamic_table,
    const ComplexFieldEntry *general_complex_field_table,
    const ComplexFieldEntry *mock_dynamic_field_table,
    unsigned int battalion_count) {

    aos_soa_transform(general, (void **)mock_dynamic_table, general_complex_field_table, mock_dynamic_field_table, battalion_count, 3,
    "direction", "animation", "hp");

}

/*static void general_static_diff_table_init(
    unsigned int obj_count,
    MockGeneralStaticTableDiff *mock_static_table_diff,
    GeneralStaticTable *mock_friendly_static_table,
    GeneralStaticTable *mock_enemy_static_table,
    const ComplexFieldEntry *mock_static_field_table_diff,
    const ComplexFieldEntry *mock_static_field_table) {

    // this function assumes that the field tables are compatiable and counter parts of each other
    
    unsigned int diff_table_fields_count = complex_field_table_fields_count(mock_static_field_table_diff);
    const char *keys[diff_table_fields_count] = {0};
    for (unsigned int i = 0; i < diff_table_fields_count; i++) {

        keys[i] = mock_static_field_table_diff[i].key;

    }

    unsigned int static_table_fields_count = complex_field_table_fields_count(mock_static_field_table);
    
    for (unsigned int o = 0; o < obj_count; o++) {
        
        for (unsigned int i = 0; i < diff_table_fields_count; i++) {

            for (unsigned int j = 0; j < static_table_fields_count; j++) {

                if (keys[i] == mock_static_field_table[j].key) {

                    void *src_friendly_ptr = (void *)((char *)(*((char *)mock_friendly_static_table + mock_static_field_table[j].offset)) + mock_static_field_table[j].size * o);
                    void *src_enemy_ptr = (void *)((char *)(*((char *)mock_enemy_static_table + mock_static_field_table[j].offset)) + mock_static_field_table[j].size * o);
                    
                    unsigned long long int src_friendly = *((unsigned long long int *)src_friendly_ptr);
                    unsigned long long int src_enemy = *((unsigned long long int *)src_enemy_ptr);
                    
                    size_t size = mock_static_field_table[j].size;
                    if (size < sizeof(unsigned long long int)) {

                        size_t shift = sizeof(unsigned long long int) - size;

                        src_friendly &= (-1ULL >> shift);
                        src_enemy &= (-1ULL >> shift);

                    }

                    void *dest = (void *)((char *)(*((char *)mock_static_table_diff + mock_static_field_table_diff.offset)) + mock_static_field_table_diff[j].size * o);
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

}*/

/*static void general_dynamic_diff_table_init(
    unsigned int obj_count,
    MockGeneralStaticTableDiff *mock_dynamic_table_diff,
    GeneralStaticTable *mock_friendly_dynamic_table,
    GeneralStaticTable *mock_enemy_dynamic_table,
    const ComplexFieldEntry *mock_dynamic_field_table_diff,
    const ComplexFieldEntry *mock_dynamic_field_table) {

    // this function assumes that the field tables are compatiable and counter parts of each other
    
    unsigned int diff_table_fields_count = complex_field_table_fields_count(mock_dynamic_field_table_diff);
    const char *keys[diff_table_fields_count] = {0};
    for (unsigned int i = 0; i < diff_table_fields_count; i++) {

        keys[i] = mock_dynamic_field_table_diff[i].key;

    }

    unsigned int dynamic_table_fields_count = complex_field_table_fields_count(mock_dynamic_field_table);
    
    for (unsigned int o = 0; o < obj_count; o++) {

        for (unsigned int i = 0; i < diff_table_fields_count; i++) {

            for (unsigned int j = 0; j < dynamic_table_fields_count; j++) {

                if (keys[i] == mock_dynamic_field_table[j].key) {

                    if (keys[i] == "threat") {

                        enum ThreatLevel threat = 0;
                        for (unsigned int k = 0; k < dynamic_table_fields_count; k++) {
                        
                            if (keys[k] == "type") {

                                enum Type friendly_type = (enum Type)(*((char *)(*((char *)mock_friendly_dynamic_table + mock_dynamic_field_table[k].offset)) + mock_dynamic_field_table[k].size * o));
                                enum Type enemy_type = (enum Type)(*((char *)(*((char *)mock_enemy_dynamic_table + mock_dynamic_field_table[k].offset)) + mock_dynamic_field_table[k].size * o));
                        
                                threat = evaluate_battalion_threat(friendly_type, enemy_type);
                                break;

                            }

                        }

                        enum ThreatLevel *dest = &((enum ThreatLevel)(*((char *)(*((char *)mock_dynamic_table_diff + mock_dynamic_field_table_diff[k].offset)) + mock_dynamic_field_table_diff[k].size * o)));
                        *dest = threat;

                        continue;

                    }

                    void *src_friendly_ptr = (void *)((char *)(*((char *)mock_friendly_dynamic_table + mock_dynamic_field_table[j].offset)) + mock_dynamic_field_table[j].size * o);
                    void *src_enemy_ptr = (void *)((char *)(*((char *)mock_enemy_dynamic_table + mock_dynamic_field_table[j].offset)) + mock_dynamic_field_table[j].size * o);
                    
                    unsigned long long int src_friendly = *((unsigned long long int *)src_friendly_ptr);
                    unsigned long long int src_enemy = *((unsigned long long int *)src_enemy_ptr);
                    
                    size_t size = mock_dynamic_field_table[j].size;
                    if (size < sizeof(unsigned long long int)) {

                        size_t shift = sizeof(unsigned long long int) - size;

                        src_friendly &= (-1ULL >> shift);
                        src_enemy &= (-1ULL >> shift);

                    }

                    void *dest = (void *)((char *)(*((char *)mock_dynamic_table_diff + mock_dynamic_field_table_diff[j].offset)) + mock_dynamic_field_table_diff[j].size * o);
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

}*/
