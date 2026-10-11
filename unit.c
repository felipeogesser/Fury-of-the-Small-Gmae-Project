#include "unit.h"
#include "unit_internal.h"
#include "animation_types.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "engine_internal.h"
#include "field_entry_internal.h"
#include "game_state_internal.h"
#include "general_internal.h"
#include "maps_internal.h"
#include "sprites_internal.h"
#include <stddef.h>

/*static const FieldEntry field_table[] = {

    #define OFFSET_OF(type, member) ((size_t) &(((type *)0)->member))
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "anim",            OFFSET_OF(Unit, anim),            SIZE_OF(Unit, anim) },
    { "sprite",          OFFSET_OF(Unit, sprite),          SIZE_OF(Unit, sprite) },
    { "id",              OFFSET_OF(Unit, id),              SIZE_OF(Unit, id) },
    { "obj_size",        0,                                sizeof(Unit) },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const FieldEntry *const unit_field_table = field_table;*/

static const ComplexFieldEntry unit_complex_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "anim",                OFFSET_OF(Unit, anim),                SIZE_OF(Unit, anim),                STRUCT, animation_state_complex_field_table },
    { "sprite",              OFFSET_OF(Unit, sprite),              SIZE_OF(Unit, sprite),              STRUCT, sprite_info_complex_field_table },
    { "id",                  OFFSET_OF(Unit, id),                  SIZE_OF(Unit, id),                  U_INT,       NULL },
    { "name",                OFFSET_OF(Unit, name),                SIZE_OF(Unit, name),                S_CHAR,      NULL },
    { "positionX",           OFFSET_OF(Unit, positionX),           SIZE_OF(Unit, positionX),           FLOAT,       NULL },
    { "positionY",           OFFSET_OF(Unit, positionY),           SIZE_OF(Unit, positionY),           FLOAT,       NULL },
    { "dimensionX",          OFFSET_OF(Unit, dimensionX),          SIZE_OF(Unit, dimensionX),          FLOAT,       NULL },
    { "dimensionY",          OFFSET_OF(Unit, dimensionY),          SIZE_OF(Unit, dimensionY),          FLOAT,       NULL },
    { "quadrantOccupiedX",   OFFSET_OF(Unit, quadrantOccupiedX),   SIZE_OF(Unit, quadrantOccupiedX),   FLOAT,       NULL },
    { "quadrantOccupiedY",   OFFSET_OF(Unit, quadrantOccupiedY),   SIZE_OF(Unit, quadrantOccupiedY),   FLOAT,       NULL },
    { "previousCol",         OFFSET_OF(Unit, previousCol),         SIZE_OF(Unit, previousCol),         S_INT,       NULL },
    { "previousRow",         OFFSET_OF(Unit, previousRow),         SIZE_OF(Unit, previousRow),         S_INT,       NULL },
    { "previousX",           OFFSET_OF(Unit, previousX),           SIZE_OF(Unit, previousX),           S_INT,       NULL },
    { "previousY",           OFFSET_OF(Unit, previousY),           SIZE_OF(Unit, previousY),           S_INT,       NULL },
    { "collision",           OFFSET_OF(Unit, collision),           SIZE_OF(Unit, collision),           BOOL,        NULL },
    { "speed",               OFFSET_OF(Unit, speed),               SIZE_OF(Unit, speed),               FLOAT,       NULL },
    { "vectorX",             OFFSET_OF(Unit, vectorX),             SIZE_OF(Unit, vectorX),             FLOAT,       NULL },
    { "vectorY",             OFFSET_OF(Unit, vectorY),             SIZE_OF(Unit, vectorY),             FLOAT,       NULL },
    { "lastTick",            OFFSET_OF(Unit, lastTick),            SIZE_OF(Unit, lastTick),            U_INT,       NULL },
    { "nextMoveDelay",       OFFSET_OF(Unit, nextMoveDelay),       SIZE_OF(Unit, nextMoveDelay),       S_INT,       NULL },
    { "currentQuadrants",    OFFSET_OF(Unit, currentQuadrants),    SIZE_OF(Unit, currentQuadrants),    S_INT,       NULL },
    { "quadrantOutOfBounds", OFFSET_OF(Unit, quadrantOutOfBounds), SIZE_OF(Unit, quadrantOutOfBounds), U_CHAR,      NULL },
    { "enemyUnitId",         OFFSET_OF(Unit, enemyUnitId),         SIZE_OF(Unit, enemyUnitId),         S_INT,       NULL },
    { "pathFound",           OFFSET_OF(Unit, pathFound),           SIZE_OF(Unit, pathFound),           BOOL,        NULL },
    { "constMovX",           OFFSET_OF(Unit, constMovX),           SIZE_OF(Unit, constMovX),           FLOAT,       NULL },
    { "constMovY",           OFFSET_OF(Unit, constMovY),           SIZE_OF(Unit, constMovY),           FLOAT,       NULL },
    { "health",              OFFSET_OF(Unit, health),              SIZE_OF(Unit, health),              FLOAT,       NULL },
    { "direction",           OFFSET_OF(Unit, direction),           SIZE_OF(Unit, direction),           S_INT,       NULL },
    { "target_idx",          OFFSET_OF(Unit, target_idx),          SIZE_OF(Unit, target_idx),          U_INT,       NULL },
    { "target_type",         OFFSET_OF(Unit, target_type),         SIZE_OF(Unit, target_type),         S_INT,       NULL },
    { "is_engaged",             OFFSET_OF(Unit, is_engaged),             SIZE_OF(Unit, is_engaged),             BOOL,        NULL },
    { "is_alive",            OFFSET_OF(Unit, is_alive),            SIZE_OF(Unit, is_alive),            BOOL,        NULL },
    { "obj_size",            0,                                    sizeof(Unit),                       NOT_DEFINED, NULL },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const ComplexFieldEntry *const unit_complex_field_table_ptr = unit_complex_field_table;

static const FieldEntry field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "anim",                OFFSET_OF(Unit, anim),                SIZE_OF(Unit, anim)                },
    { "sprite",              OFFSET_OF(Unit, sprite),              SIZE_OF(Unit, sprite)              },
    { "id",                  OFFSET_OF(Unit, id),                  SIZE_OF(Unit, id)                  },
    { "name",                OFFSET_OF(Unit, name),                SIZE_OF(Unit, name)                },
    { "positionX",           OFFSET_OF(Unit, positionX),           SIZE_OF(Unit, positionX)           },
    { "positionY",           OFFSET_OF(Unit, positionY),           SIZE_OF(Unit, positionY)           },
    { "dimensionX",          OFFSET_OF(Unit, dimensionX),          SIZE_OF(Unit, dimensionX)          },
    { "dimensionY",          OFFSET_OF(Unit, dimensionY),          SIZE_OF(Unit, dimensionY)          },
    { "quadrantOccupiedX",   OFFSET_OF(Unit, quadrantOccupiedX),   SIZE_OF(Unit, quadrantOccupiedX)   },
    { "quadrantOccupiedY",   OFFSET_OF(Unit, quadrantOccupiedY),   SIZE_OF(Unit, quadrantOccupiedY)   },
    { "previousCol",         OFFSET_OF(Unit, previousCol),         SIZE_OF(Unit, previousCol)         },
    { "previousRow",         OFFSET_OF(Unit, previousRow),         SIZE_OF(Unit, previousRow)         },
    { "previousX",           OFFSET_OF(Unit, previousX),           SIZE_OF(Unit, previousX)           },
    { "previousY",           OFFSET_OF(Unit, previousY),           SIZE_OF(Unit, previousY)           },
    { "collision",           OFFSET_OF(Unit, collision),           SIZE_OF(Unit, collision)           },
    { "speed",               OFFSET_OF(Unit, speed),               SIZE_OF(Unit, speed)               },
    { "vectorX",             OFFSET_OF(Unit, vectorX),             SIZE_OF(Unit, vectorX)             },
    { "vectorY",             OFFSET_OF(Unit, vectorY),             SIZE_OF(Unit, vectorY)             },
    { "lastTick",            OFFSET_OF(Unit, lastTick),            SIZE_OF(Unit, lastTick)            },
    { "nextMoveDelay",       OFFSET_OF(Unit, nextMoveDelay),       SIZE_OF(Unit, nextMoveDelay)       },
    { "currentQuadrants",    OFFSET_OF(Unit, currentQuadrants),    SIZE_OF(Unit, currentQuadrants)    },
    { "quadrantOutOfBounds", OFFSET_OF(Unit, quadrantOutOfBounds), SIZE_OF(Unit, quadrantOutOfBounds) },
    { "enemyUnitId",         OFFSET_OF(Unit, enemyUnitId),         SIZE_OF(Unit, enemyUnitId)         },
    { "pathFound",           OFFSET_OF(Unit, pathFound),           SIZE_OF(Unit, pathFound)           },
    { "constMovX",           OFFSET_OF(Unit, constMovX),           SIZE_OF(Unit, constMovX)           },
    { "constMovY",           OFFSET_OF(Unit, constMovY),           SIZE_OF(Unit, constMovY)           },
    { "health",              OFFSET_OF(Unit, health),              SIZE_OF(Unit, health)              },
    { "direction",           OFFSET_OF(Unit, direction),           SIZE_OF(Unit, direction)           },
    { "target_idx",          OFFSET_OF(Unit, target_idx),          SIZE_OF(Unit, target_idx)          },
    { "target_type",         OFFSET_OF(Unit, target_type),         SIZE_OF(Unit, target_type)         },
    { "is_engaged",             OFFSET_OF(Unit, is_engaged),             SIZE_OF(Unit, is_engaged)             },
    { "is_alive",            OFFSET_OF(Unit, is_alive),            SIZE_OF(Unit, is_alive)            },
    { "obj_size",            0,                                    sizeof(Unit)                       },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const FieldEntry *const unit_field_table = field_table;

// private prototypes
void update_units_position(Unit *unit, unsigned int unit_count, signed int x, signed int y);

static void set_units_id(Unit *unit, unsigned int battalion_size);
static void set_units_dimension(Unit *unit, unsigned int battalion_size);
static void set_units_sprite_and_animation(Unit *unit, General *general, unsigned int battalion_size);
static void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion, unsigned int x, unsigned int y, _Bool is_enemy_army);


void init_units(Battalion *battalion, unsigned int x, unsigned int y, _Bool is_enemy_army) {
    // this function expects to be called only when generals are already initialized
    General *general = battalion->general;
    Unit *unit = battalion->unit;
    unsigned int battalion_size = battalion->unit_count;

    set_units_id(unit, battalion_size);
    set_units_sprite_and_animation(unit, general, battalion_size);
    set_units_dimension(unit, battalion_size);
    set_units_position(unit, battalion_size, battalion, x, y, is_enemy_army);

}

void update_units(Armies *armies, GameState *game) {
    
    Unit *unit = armies->army->general->battalions->unit;

    for (unsigned int i = 0; i < game->unit_created_count; i++) {
        unit[i].positionX += unit[i].vectorX * game->delta;
        unit[i].positionY += unit[i].vectorY * game->delta;
    }

}

void update_units_position(Unit *unit, unsigned int unit_count, signed int x, signed int y) {

    for (unsigned int i = 0; i < unit_count; i++) {

        unit[i].positionX = x;
        unit[i].positionY = y;

    }

}

static void set_units_id(Unit *unit, unsigned int battalion_size) {

    static unsigned int id = 1;
    for (unsigned int i = 0; i < battalion_size; i++) {

        unit[i].id = id;
        id++;

    }

}

static void set_units_sprite_and_animation(Unit *unit, General *general, unsigned int battalion_size) {

    for (unsigned int i = 0; i < battalion_size; i++) {

        unit[i].anim.animation = IDLE;
        unit[i].anim.frames_count = engine.sprite_pack->sprite[general->units_type][IDLE].frames_count;
        unit[i].anim.current_frame = (unsigned char)(rand() % unit[i].anim.frames_count);
        unit[i].sprite.type = general->units_type;
        unit[i].sprite.w = engine.sprite_pack->sprite[general->units_type][IDLE].width / unit[i].anim.frames_count;
        unit[i].sprite.h = engine.sprite_pack->sprite[general->units_type][IDLE].height;

    }

}

static void set_units_dimension(Unit *unit, unsigned int battalion_size) {

    for (unsigned int i = 0; i < battalion_size; i++) {

        unit[i].dimensionX = unit[i].sprite.w;
        unit[i].dimensionY = unit[i].sprite.h;

    }

}

static void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion, unsigned int x, unsigned int y, _Bool is_enemy_army) {

    unsigned int formation_height = battalion->unit_formation_height;
    unsigned int formation_width = ceilf(battalion_size / battalion->unit_formation_height);

    unsigned int pad_x = battalion->padding_between_units_x;
    unsigned int pad_y = battalion->padding_between_units_y;

    unsigned int pos_x = x;
    unsigned int pos_y = y;
    unsigned int dim_x = unit->dimensionX;
    unsigned int dim_y = unit->dimensionY;

    pos_x -= (dim_x + pad_x) * !is_enemy_army;
    pos_x += (dim_x + pad_x) * formation_width * is_enemy_army;
    pos_y += (dim_y / 2 - (dim_y + pad_y) * battalion_size / 2) * (battalion_size < formation_height);
    pos_y += (dim_y / 2 - (dim_y + pad_y) * formation_height / 2) * !(battalion_size < formation_height);

    unsigned int idx = 0;
    for (unsigned int i = 0; i < formation_width; i++) {

        for (unsigned int j = 0; j < formation_height; j++) {

            unit[idx].positionX = pos_x - i * (dim_x + pad_x);
            unit[idx].positionY = pos_y + j * (dim_y + pad_y);
            idx++;
            if (idx == battalion_size) return;
            
        }

    }

}
