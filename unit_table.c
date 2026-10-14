#include "unit_table.h"
#include "unit_table_internal.h"
#include "unit_types.h"
#include "animation.h"
#include "animation_types.h"
#include "aos_soa_transformer.h"
#include "battlefield_types.h"
#include "camera.h"
#include "collision.h"
#include "combat.h"
#include "direction_types.h"
#include "engine_internal.h"
#include "field_entry.h"
#include "formation.h"
#include "memory_arena.h"
#include "spreadsheet.h"
#include "spreadsheet_internal.h"
#include "sprites_internal.h"
#include "steering.h"
#include "table_internal.h"
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

// private prototypes
static void unit_dynamic_table_init(
    Unit *unit,
    UnitDynamicTable *mock_dynamic_table,
    const ComplexFieldEntry *unit_field_table_ptr,
    const ComplexFieldEntry *mock_dynamic_field_table,
    unsigned int unit_count);
static void allocate_mock_soa_memory(
    unsigned int obj_count,
    UnitDynamicTable *mock_friendly_dynamic_table,
    UnitDynamicTable *mock_enemy_dynamic_table);
static void allocate_mem_for_mock(unsigned int obj_count, const ComplexFieldEntry *field_table, void *mock);
static void update_speed(Table *tables, unsigned int unit_count, unsigned int battalion_size);
static void resolve_attack(Table *tables, unsigned int unit_count, unsigned int battalion_size, float dt);
static void update_position(Table *tables, unsigned int battalion_count, unsigned int battalion_size, unsigned int unit_count,enum ArmyBattlefieldSide side);
static void update_direction(Table *tables, unsigned int unit_count);
static void animation_table_update(Table *tables, unsigned int unit_count, unsigned int battalion_size);
static _Bool try_engage_enemy(float fx, float fy, enum TargetType *target_type, unsigned int *target_idx, Table *tables, unsigned int battalion_count, unsigned int battalion_size, unsigned int engaged_battalion_idx);
_Bool is_enemy_in_range(float fx, float fy, float range, float tx, float ty);
static void routing_units(Table *tables, unsigned int unit_count, unsigned int battalion_size, enum ArmyBattlefieldSide side);
static float direction_from_vec(float vec_x, float vec_y, float fallback);
static float wrap_direction(float dir);

static void *unit_table_start_address_alloc = NULL;

static const ComplexFieldEntry unit_dynamic_table_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "direction",      OFFSET_OF(UnitDynamicTable, direction),      SIZE_OF(UnitDynamicTable, direction),      S_INT,       NULL },
    { "target_idx",     OFFSET_OF(UnitDynamicTable, target_idx),     SIZE_OF(UnitDynamicTable, target_idx),     U_INT,       NULL },
    { "target_type",    OFFSET_OF(UnitDynamicTable, target_type),    SIZE_OF(UnitDynamicTable, target_type),    S_INT,       NULL },
    { "vec_x",          OFFSET_OF(UnitDynamicTable, vec_x),          SIZE_OF(UnitDynamicTable, vec_x),          FLOAT,       NULL },
    { "vec_y",          OFFSET_OF(UnitDynamicTable, vec_y),          SIZE_OF(UnitDynamicTable, vec_y),          FLOAT,       NULL },
    { "x",              OFFSET_OF(UnitDynamicTable, x),              SIZE_OF(UnitDynamicTable, x),              FLOAT,       NULL },
    { "y",              OFFSET_OF(UnitDynamicTable, y),              SIZE_OF(UnitDynamicTable, y),              FLOAT,       NULL },
    { "hp",             OFFSET_OF(UnitDynamicTable, hp),             SIZE_OF(UnitDynamicTable, hp),             FLOAT,       NULL },
    { "attack_timer",             OFFSET_OF(UnitDynamicTable, attack_timer),             SIZE_OF(UnitDynamicTable, attack_timer),             FLOAT,       NULL },
    { "animation",      OFFSET_OF(UnitDynamicTable, animation),      SIZE_OF(UnitDynamicTable, animation),      S_INT,       NULL },
    { "current_frame",  OFFSET_OF(UnitDynamicTable, current_frame),  SIZE_OF(UnitDynamicTable, current_frame),  U_CHAR,      NULL },
    { "is_target_in_range",        OFFSET_OF(UnitDynamicTable, is_target_in_range),        SIZE_OF(UnitDynamicTable, is_target_in_range),        BOOL,        NULL },
    { "is_engaged",        OFFSET_OF(UnitDynamicTable, is_engaged),        SIZE_OF(UnitDynamicTable, is_engaged),        BOOL,        NULL },
    { "is_present",       OFFSET_OF(UnitDynamicTable, is_present),       SIZE_OF(UnitDynamicTable, is_present),       BOOL,        NULL },
    { "is_attacking",       OFFSET_OF(UnitDynamicTable, is_attacking),       SIZE_OF(UnitDynamicTable, is_attacking),       BOOL,        NULL },
    { "obj_size",       0,                                           sizeof(UnitDynamicTable),                  NOT_DEFINED, NULL },

    #undef OFFSET_OF
    #undef SIZE_OF

};
const ComplexFieldEntry *const unit_dynamic_table_field_table_ptr = unit_dynamic_table_field_table;

void unit_table_init(Table *player_tables, Table *enemy_tables, Unit *player_unit, Unit *enemy_unit, unsigned int total_battalion_count, unsigned int battalion_size) {

    UnitDynamicTable mock_friendly_dynamic_table = {0};
    UnitDynamicTable mock_enemy_dynamic_table = {0};

    unsigned int battalion_count = total_battalion_count / 2; // half total battalion count as each side always has the same battalion count

    allocate_mock_soa_memory(
        battalion_count * battalion_size,
        &mock_friendly_dynamic_table,
        &mock_enemy_dynamic_table
    );

    unit_dynamic_table_init(player_unit, &mock_friendly_dynamic_table, unit_complex_field_table_ptr, unit_dynamic_table_field_table_ptr, battalion_count * battalion_size);
    unit_dynamic_table_init(enemy_unit, &mock_enemy_dynamic_table, unit_complex_field_table_ptr, unit_dynamic_table_field_table_ptr, battalion_count * battalion_size);

    /*UnitComparisonTable unit_tables = {
        .friendly_dynamic = (UnitDynamicTable)mock_friendly_dynamic_table,
        .enemy_dynamic = (UnitDynamicTable)mock_enemy_dynamic_table
    };*/

    player_tables->unit_tables.friendly_dynamic = mock_friendly_dynamic_table;
    player_tables->unit_tables.enemy_dynamic = mock_enemy_dynamic_table;
    enemy_tables->unit_tables.friendly_dynamic = mock_enemy_dynamic_table;
    enemy_tables->unit_tables.enemy_dynamic = mock_friendly_dynamic_table;

}

void unit_table_update(Table *tables, unsigned int total_battalion_count, unsigned int battalion_size, enum ArmyBattlefieldSide side, float dt) {

    unsigned int battalion_count = total_battalion_count / 2; 
    
    BattalionTable *battalion_tables = &tables->battalion_tables;
    GeneralTable *general_tables = &tables->general_tables;
    UnitTable *unit_tables = &tables->unit_tables;
    unsigned int unit_count = battalion_size * battalion_count;
    for (unsigned int i = 0; i < unit_count; i++) {
        
        unsigned int battalion_idx = i / battalion_size;
        unsigned int friendly_idx = i;
        unit_tables->friendly_dynamic.is_present[friendly_idx] = unit_tables->friendly_dynamic.hp[friendly_idx] > 0;
        _Bool is_present = unit_tables->friendly_dynamic.is_present[friendly_idx];
        if (is_present) {
        
            _Bool is_engaged = unit_tables->friendly_dynamic.is_engaged[friendly_idx];
            if (is_engaged) {

                unsigned int target = unit_tables->friendly_dynamic.target_idx[friendly_idx];
                enum TargetType target_type = unit_tables->friendly_dynamic.target_type[friendly_idx];
                if (target_type == TARGET_GENERAL) {

                    _Bool is_target_alive = (general_tables->enemy_dynamic.hp[target] > 0);
                    if (!is_target_alive) {
                        continue;
                    }

                    float fx = unit_tables->friendly_dynamic.x[friendly_idx];
                    float fy = unit_tables->friendly_dynamic.y[friendly_idx];
                    float tx = general_tables->enemy_dynamic.x[target];
                    float ty = general_tables->enemy_dynamic.y[target];
                    float range = battalion_tables->friendly_static.range[battalion_idx];
                    _Bool in_range = is_enemy_in_range(fx, fy, range, tx, ty);
                    unit_tables->friendly_dynamic.is_target_in_range[friendly_idx] = in_range;
                    if (in_range) {
                        unsigned int friendly_melee_attack = battalion_tables->friendly_dynamic.meele_attack[friendly_idx];
                        unsigned int enemy_melee_defense = general_tables->enemy_dynamic.meele_defense[target];
                        _Bool is_charging = battalion_tables->friendly_dynamic.is_charging[battalion_idx];
                        if (is_charging) {
                            friendly_melee_attack += battalion_tables->friendly_static.charge_bonus[battalion_idx];
                        }
                        unsigned int chance = hit_chance(friendly_melee_attack, enemy_melee_defense);
                        _Bool hit = roll_hit(chance);
                        if (hit) {
                            float *const hp = &general_tables->enemy_dynamic.hp[target];
                            float armor = general_tables->enemy_static.armor[target];
                            float dmg = battalion_tables->friendly_dynamic.strength[battalion_idx];
                            if (is_charging) {
                                dmg += battalion_tables->friendly_static.charge_bonus[battalion_idx];
                            }
                            dmg = apply_armor(dmg, armor);
                            *hp -= dmg;
                        }
                    
                    }

                } else if (target_type == TARGET_UNIT) {
                    
                    _Bool is_target_alive = (unit_tables->enemy_dynamic.hp[target] > 0);
                    if (!is_target_alive) {
                        continue;
                    }

                    float fx = unit_tables->friendly_dynamic.x[friendly_idx];
                    float fy = unit_tables->friendly_dynamic.y[friendly_idx];
                    float tx = unit_tables->enemy_dynamic.x[target];
                    float ty = unit_tables->enemy_dynamic.y[target];
                    float range = battalion_tables->friendly_static.range[battalion_idx];
                    _Bool in_range = is_enemy_in_range(fx, fy, range, tx, ty);
                    unit_tables->friendly_dynamic.is_target_in_range[friendly_idx] = in_range;
                    if (in_range) {
                        unsigned int friendly_melee_attack = battalion_tables->friendly_dynamic.meele_attack[battalion_idx];
                        unsigned int enemy_melee_defense = battalion_tables->enemy_dynamic.meele_defense[target / battalion_size];
                        unsigned int chance = hit_chance(friendly_melee_attack, enemy_melee_defense);
                        _Bool hit = roll_hit(chance);
                        if (hit) {
                            float *const hp = &unit_tables->enemy_dynamic.hp[target];
                            float armor = battalion_tables->enemy_static.armor[target / battalion_size];
                            float dmg = battalion_tables->friendly_dynamic.strength[battalion_idx];
                            dmg = apply_armor(dmg, armor);
                            *hp -= dmg;
                        }
                    
                    }

                } else {

                    fprintf(stderr, "unknown target\n");
                    exit(EXIT_FAILURE);

                }

            } else {

                unsigned int engaged_battalion_idx = battalion_tables->friendly_dynamic.target_battalion_idx[battalion_idx];
                _Bool engage_successful = try_engage_enemy(
                    unit_tables->friendly_dynamic.x[friendly_idx],
                    unit_tables->friendly_dynamic.y[friendly_idx],
                    &unit_tables->friendly_dynamic.target_type[friendly_idx],
                    &unit_tables->friendly_dynamic.target_idx[friendly_idx],
                    tables,
                    battalion_count,
                    battalion_size,
                    engaged_battalion_idx
                );
                unit_tables->friendly_dynamic.is_engaged[friendly_idx] = engage_successful;

            }

        }

    }

    update_speed(tables, unit_count, battalion_size);
    resolve_attack(tables, unit_count, battalion_size, dt);
    update_position(tables, battalion_count, battalion_size, unit_count, side);
    update_direction(tables, unit_count);
    animation_table_update(tables, unit_count, battalion_size);

}

void unit_table_render(Table *tables, unsigned int total_battalion_count, unsigned int battalion_size) {

    unsigned int battalion_count = total_battalion_count / 2; 

    unsigned int unit_count = battalion_size * battalion_count;
    for (unsigned int i = 0; i < unit_count; i++) {

        const _Bool is_present = tables->unit_tables.friendly_dynamic.is_present[i];
        if (!is_present) continue;
        const enum Direction direction = tables->unit_tables.friendly_dynamic.direction[i];
        const float x = tables->unit_tables.friendly_dynamic.x[i];
        const float y = tables->unit_tables.friendly_dynamic.y[i];
        const enum BattalionType battalion_type = tables->battalion_tables.friendly_static.battalion_type[i / battalion_size];
        //enum BattalionState battalion_state = tables->battalion_tables.friendly_static.state[i];
        const unsigned char current_frame = tables->unit_tables.friendly_dynamic.current_frame[i];
        const enum Animation anim_type = tables->unit_tables.friendly_dynamic.animation[i];
        const enum Sprites sprite_type = get_battalion_spreadsheet(battalion_type).sprite_type;
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

}

void unit_table_destroy(void) {

    memory_arena_reset_to_address(unit_table_start_address_alloc);

}

static void update_speed(Table *tables, unsigned int unit_count, unsigned int battalion_size) {

    for(unsigned int i = 0; i < unit_count; i++) {

        float speed = tables->battalion_tables.friendly_dynamic.speed[i / battalion_size];
        float charge_bonus = tables->battalion_tables.friendly_static.charge_bonus[i / battalion_size];        
        if(tables->battalion_tables.friendly_dynamic.movement_state[i / battalion_size] == CHARGING) {

            speed += charge_bonus / speed;

        }

        speed *= 0.01f;

        if (tables->unit_tables.friendly_dynamic.is_engaged) {

            tables->unit_tables.friendly_dynamic.vec_x[i] = 0;
            tables->unit_tables.friendly_dynamic.vec_y[i] = 0;

        } else {

            tables->unit_tables.friendly_dynamic.vec_x[i] += speed;
            tables->unit_tables.friendly_dynamic.vec_y[i] += speed;
            
        }

    }

}

static void resolve_attack(Table *tables, unsigned int unit_count, unsigned int battalion_size, float dt) {

    for (unsigned int i = 0; i < unit_count; i++) {

        if (!tables->unit_tables.friendly_dynamic.is_engaged[i]) return;
        float attack_timer = tables->unit_tables.friendly_dynamic.attack_timer[i];
        attack_timer -= dt;
        if (attack_timer > 0.0f) return;

        float attack_speed = tables->battalion_tables.friendly_static.attack_speed[i / battalion_size];
        tables->unit_tables.friendly_dynamic.attack_timer[i] = attack_speed;

        float strength = tables->battalion_tables.friendly_dynamic.strength[i / battalion_size];
        float charge_bonus = tables->battalion_tables.friendly_static.charge_bonus[i / battalion_size];
        float meele_attack = tables->battalion_tables.friendly_dynamic.meele_attack[i / battalion_size];
        unsigned int target_idx = tables->unit_tables.friendly_dynamic.target_idx[i];
        enum TargetType target_type = tables->unit_tables.friendly_dynamic.target_type[i];
        _Bool is_armor_piercing = tables->battalion_tables.friendly_static.is_armor_piercing[i / battalion_size];
        float *target_hp = NULL;
        float target_meele_defense = 0.0f;
        float target_armor = 0.0f;
        _Bool is_target_large = 0;
        
        if (target_type == TARGET_UNIT) {

            target_hp = &tables->unit_tables.enemy_dynamic.hp[target_idx];
            target_meele_defense = tables->battalion_tables.enemy_dynamic.meele_defense[target_idx / battalion_size];
            target_armor = tables->battalion_tables.enemy_static.armor[target_idx / battalion_size];
            is_target_large = tables->battalion_tables.enemy_static.is_large[target_idx / battalion_size];

        } else if (target_type == TARGET_GENERAL) {

            target_hp = &tables->general_tables.enemy_dynamic.hp[target_idx];
            target_meele_defense = tables->general_tables.enemy_dynamic.meele_defense[target_idx];
            target_armor = tables->general_tables.enemy_static.armor[target_idx];
            is_target_large = tables->general_tables.enemy_static.is_large[target_idx];

        }

        if (*target_hp <= 0.0f) {

            tables->unit_tables.friendly_dynamic.is_engaged[i] = false;
            tables->unit_tables.friendly_dynamic.target_idx[i] = -1U;
            tables->unit_tables.friendly_dynamic.target_type[i] = UNKNOWN_TARGET;
            return;
            
        }

        _Bool is_charging = tables->battalion_tables.friendly_dynamic.is_charging[i / battalion_size];
        float damage = strength + charge_bonus * is_charging;
        damage *= (is_target_large) ? 1.5f : 1.0f;
        target_armor = (is_armor_piercing) ? target_armor / 2 : target_armor;
        damage = apply_armor(damage, target_armor);
        float chance = hit_chance(meele_attack, target_meele_defense);
        _Bool hit = roll_hit(chance);

        if (hit) *target_hp -= damage;

    }

}

static void animation_table_update(Table *tables, unsigned int unit_count, unsigned int battalion_size) {
    
    _Bool advance_frame = animation_tick();
    if (!advance_frame) return;

    for (unsigned int i = 0; i < unit_count; i++) {

        const enum BattalionType battalion_type = tables->battalion_tables.friendly_static.battalion_type[i / battalion_size];
        const enum BattalionMovementMode movement_state = tables->battalion_tables.friendly_dynamic.movement_state[i / battalion_size];
        const enum BattalionLifecycle lifecycle_state = tables->battalion_tables.friendly_dynamic.lifecycle_state[i / battalion_size];
        const _Bool is_engaged = tables->unit_tables.friendly_dynamic.is_engaged[i];
        const _Bool is_target_in_range = tables->unit_tables.friendly_dynamic.is_target_in_range[i];
        unsigned char current_frame = tables->unit_tables.friendly_dynamic.current_frame[i];
        const enum Sprites sprite_type = get_battalion_spreadsheet(battalion_type).sprite_type;
        enum Animation anim = tables->unit_tables.friendly_dynamic.animation[i];
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
    
        tables->unit_tables.friendly_dynamic.animation[i] = anim;
        tables->unit_tables.friendly_dynamic.current_frame[i] = current_frame;

    }

}

static void update_position(Table *tables, unsigned int battalion_count, unsigned int battalion_size, unsigned int unit_count, enum ArmyBattlefieldSide side) {

    #define DISTANCE 20.0f // DISTANCE_BETWEEN_GENERAL_AND_BATTALION_CENTROID
    #define UNIT_WIDTH 24.0f
    #define UNIT_HEIGHT 24.0f
    #define PADDING_X 48.0f
    #define PADDING_Y 20.0f
    #define MAX_SPEED 20.0f

    formation_update(tables,
                battalion_count, unit_count, battalion_size,
                UNIT_WIDTH, UNIT_HEIGHT, PADDING_X, PADDING_Y,
                DISTANCE, MAX_SPEED);
    #undef DISTANCE

    steer_headless_units(
        tables, battalion_count, unit_count, battalion_size,
        UNIT_WIDTH, UNIT_HEIGHT, PADDING_X, PADDING_Y, MAX_SPEED);
    #undef UNIT_WIDTH
    #undef UNIT_HEIGHT
    #undef PADDING_X
    #undef PADDING_Y
    #undef MAX_SPEED

    #define MIN_DIST 36.0f
    #define PASSES 4U

    float *x = tables->unit_tables.friendly_dynamic.x;
    float *y = tables->unit_tables.friendly_dynamic.y;
    const _Bool *is_present = tables->unit_tables.friendly_dynamic.is_present;
    resolve_collisions(x, y, is_present, unit_count, MIN_DIST, PASSES);

    float *ax = tables->unit_tables.friendly_dynamic.x;
    float *ay = tables->unit_tables.friendly_dynamic.y;
    const _Bool *a_is_present = tables->unit_tables.friendly_dynamic.is_present;
    unsigned int a_count = unit_count;
    float *bx = tables->unit_tables.enemy_dynamic.x;
    float *by = tables->unit_tables.enemy_dynamic.y;
    const _Bool *b_is_present = tables->unit_tables.enemy_dynamic.is_present;
    unsigned int b_count = unit_count;
    resolve_collisions_between(
        ax, ay, a_is_present, a_count,
        bx, by, b_is_present, b_count,
        MIN_DIST, PASSES);
    #undef MIN_DIST
    #undef PASSES

    routing_units(tables, unit_count, battalion_size, side);

}

static void routing_units(Table *tables, unsigned int unit_count, unsigned int battalion_size, enum ArmyBattlefieldSide side) {

    for (unsigned int i = 0; i < unit_count; i++) {

        float speed = tables->battalion_tables.friendly_dynamic.speed[i / battalion_size];

        if (tables->battalion_tables.friendly_dynamic.lifecycle_state[i / battalion_size] == ROUTING) {

            if (side == ARMY_SIDE_LEFT) {

                tables->unit_tables.friendly_dynamic.x[i] = -speed * 0.01f;

            } else {

                tables->unit_tables.friendly_dynamic.y[i] = speed * 0.01f;

            }

        }

    }

}

static void update_direction(Table *tables, unsigned int unit_count) {

    for(unsigned int i = 0; i < unit_count; i++) {

        float vx = tables->unit_tables.friendly_dynamic.vec_x[i];
        float vy = tables->unit_tables.friendly_dynamic.vec_y[i];
        enum Direction direction = tables->unit_tables.friendly_dynamic.direction[i];
        float dir = direction_from_vec(vx, vy, direction == LEFT ? 2.0f : 0.0f);
        direction = (dir < 2.0f) ? RIGHT : LEFT;
        tables->unit_tables.friendly_dynamic.direction[i] = direction;

    }

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

static _Bool try_engage_enemy(float fx, float fy, enum TargetType *target_type, unsigned int *target_idx, Table *tables, unsigned int battalion_count, unsigned int battalion_size, unsigned int engaged_battalion_idx) {

    #define MEELE_SEARCH_RANGE 100

    _Bool found_enemy = false;
    enum TargetType type = UNKNOWN_TARGET;
    unsigned int target = -1U;
    float smallest_dist_recorded = MEELE_SEARCH_RANGE * MEELE_SEARCH_RANGE;

    for (unsigned int i = 0; i < battalion_size; i++) {

        unsigned int idx = engaged_battalion_idx * battalion_size + i;

        _Bool is_present = tables->unit_tables.enemy_dynamic.is_present[idx];
        if (!is_present) continue;

        float tx = tables->unit_tables.enemy_dynamic.x[idx];
        float ty = tables->unit_tables.enemy_dynamic.y[idx];

        if (tx == 0 || ty == 0) continue;
        float dx = tx - fx;
        float dy = ty - fy;
        float dist2 = dx * dx + dy * dy;
        if (dist2 <= smallest_dist_recorded) {

            found_enemy = true;
            smallest_dist_recorded = dist2;
            target = idx;
            type = TARGET_UNIT;

        }

    }

    for (unsigned int i = 0; i < battalion_count; i++) {

        _Bool is_present = tables->general_tables.enemy_dynamic.is_present[i];
        if (!is_present) continue;

        float tx = tables->general_tables.enemy_dynamic.x[i];
        float ty = tables->general_tables.enemy_dynamic.y[i];

        float dx = tx - fx;
        float dy = ty - fy;
        float dist2 = dx * dx + dy * dy;
        if (dist2 <= smallest_dist_recorded) {

            smallest_dist_recorded = dist2;
            target = i;
            type = TARGET_GENERAL;

        }

    }

    *target_idx = target;

    if (found_enemy) {
        
        *target_type = type;
        return true;

    } else {

        *target_type = UNKNOWN_TARGET;

    }

    return false;

}

/*void try_engage_enemy(float fx, float fy, _Bool is_ranged, enum TargetType *target_type, unsigned int *target_idx) {

    if (is_ranged) {

    } else {

        #define MEELE_SEARCH_RANGE 300

        for (unsigned int i = 0; i < enemy_battalion_count; i++) {


            float tx = battalion_tables.enemy_dynamic.x[i];
            float ty = battalion_tables.enemy_dynamic.y[i];

            float dx = tx - fx;
            float dy = ty - fy;
            if (dx * dx + dy * dy) <= (MEELE_SEARCH_RANGE * MEELE_SEARCH_RANGE) {

                if ()

            }

        }

    }

}*/

_Bool is_enemy_in_range(float fx, float fy, float range, float tx, float ty) {

    float dx = tx - fx;
    float dy = ty - fy;
    return (dx * dx + dy * dy) <= (range * range);

}

static void unit_dynamic_table_init(
    Unit *unit,
    UnitDynamicTable *mock_dynamic_table,
    const ComplexFieldEntry *unit_field_table_ptr,
    const ComplexFieldEntry *mock_dynamic_field_table,
    unsigned int unit_count) {

    aos_soa_transform(unit, (void **)mock_dynamic_table, unit_field_table_ptr, mock_dynamic_field_table, unit_count, 3,
    "hp", "x", "y");

}

static void allocate_mock_soa_memory(
    unsigned int obj_count,
    UnitDynamicTable *mock_friendly_dynamic_table,
    UnitDynamicTable *mock_enemy_dynamic_table) {

    unit_table_start_address_alloc = memory_arena_current_pointer();
    allocate_mem_for_mock(obj_count, unit_dynamic_table_field_table_ptr, mock_friendly_dynamic_table);
    allocate_mem_for_mock(obj_count, unit_dynamic_table_field_table_ptr, mock_enemy_dynamic_table);

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
