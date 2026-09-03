#include "unit.h"
#include "unit_internal.h"
#include "animation_types.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "general_internal.h"
#include "sprites_internal.h"
#include <stdio.h>

#define OFFSET_OF(type, member) ((size_t) &(((type *)0)->member))
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))

const FieldEntry unit_field_table[] = {
    { "anim",            OFFSET_OF(Unit, anim),            SIZE_OF(Unit, anim) },
    { "sprite",          OFFSET_OF(Unit, sprite),          SIZE_OF(Unit, sprite) },
    { "id",              OFFSET_OF(Unit, id),              SIZE_OF(Unit, id) },
    { "obj_size",  0,                     sizeof(Unit) },
};
const size_t unit_field_table_count =
    sizeof(unit_field_table) / sizeof(unit_field_table[0]);
#undef OFFSET_OF
#undef SIZE_OF

// private prototypes
void update_units_position(Unit *unit, unsigned int unit_count, signed int x, signed int y);

static void set_units_id(Unit *unit, unsigned int battalion_size);
static void set_units_dimension(Unit *unit, unsigned int battalion_size);
static void set_units_sprite_and_animation(Unit *unit, General *general, unsigned int battalion_size);
//static void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion);


void init_units(Battalion *battalion) {
    // this function expects to be called only when generals are already initialized
    General *general = battalion->general;
    Unit *unit = battalion->unit;
    unsigned int battalion_size = battalion->unit_count;

    set_units_id(unit, battalion_size);
    set_units_sprite_and_animation(unit, general, battalion_size);
    set_units_dimension(unit, battalion_size);
    set_units_position(unit, battalion_size, battalion);

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

void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion) {

    unsigned int formation_height = battalion->unit_formation_height;
    unsigned int formation_width = ceilf(battalion_size / battalion->unit_formation_height);
    unsigned int battalion_pos_x = battalion->initial_map_placement_x;
    unsigned int battalion_pos_y = battalion->initial_map_placement_y;
    unsigned int padding_x = battalion->padding_between_units_x;
    unsigned int padding_y = battalion->padding_between_units_y;
    unsigned int idx = 0;
    for (unsigned int i = 0; i < formation_width; i++) {

        for (unsigned int j = 0; j < formation_height; j++) {

            unsigned int x = battalion_pos_x + padding_x + (unit[idx].dimensionX * i);
            unsigned int y = battalion_pos_y + padding_y + (unit[idx].dimensionY * j);
            unit[idx].positionX = x;
            unit[idx].positionY = y;
            idx++;
            if (idx == battalion_size) return;
            
        }

    }

}