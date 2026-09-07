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
#include "maps_internal.h"
#include "battlefield_internal.h"
#include "battleplan_internal.h"
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


void init_units(Battalion *battalion, Grid *grid, OccupiedCell *occupied_cell) {
    // this function expects to be called only when generals are already initialized
    General *general = battalion->general;
    Unit *unit = battalion->unit;
    unsigned int battalion_size = battalion->unit_count;

    set_units_id(unit, battalion_size);
    set_units_sprite_and_animation(unit, general, battalion_size);
    set_units_dimension(unit, battalion_size);
    set_units_position(unit, battalion_size, battalion, grid, occupied_cell);

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

void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion, Grid *grid, OccupiedCell *occupied_cell) {

    unsigned int formation_height = battalion->unit_formation_height;
    unsigned int formation_width = ceilf(battalion_size / battalion->unit_formation_height);

    unsigned int unit_padding_x = battalion->padding_between_units_x;
    unsigned int unit_padding_y = battalion->padding_between_units_y;

    unsigned int cell_width = (engine.map->mapSizeX / 2 - engine.battlefield->padding.in_between_armies / 2 - engine.battlefield->padding.left) / grid->dimension.x;
    unsigned int cell_height = (engine.map->mapSizeY - engine.battlefield->padding.bottom - engine.battlefield->padding.top ) / grid->dimension.y;

    unsigned int cell_position_x = engine.battlefield->padding.left + occupied_cell->x * cell_width;
    unsigned int cell_position_y = engine.battlefield->padding.top + occupied_cell->y * cell_height;

    unsigned int cell_inner_padding_top = 0;
    if (battalion_size < formation_height) {

        cell_inner_padding_top = ((cell_height / (unit->sprite.h + unit_padding_y)) * (formation_height - battalion_size)) / 2;

    } else {

        cell_inner_padding_top = (cell_height - formation_height * (unit->sprite.h + unit_padding_y)) / 2;

    }

    unsigned int cell_inner_padding_left = 0;
    cell_inner_padding_left = (cell_width - formation_width * (unit->sprite.w + unit_padding_x)) / 2;


    unsigned int idx = 0;
    for (unsigned int i = 0; i < formation_width; i++) {

        for (unsigned int j = 0; j < formation_height; j++) {

            unsigned int x = cell_position_x + cell_inner_padding_left + unit_padding_x + (unit[idx].dimensionX * i);
            unsigned int y = cell_position_y + cell_inner_padding_top + unit_padding_y + (unit[idx].dimensionY * j);
            unit[idx].positionX = x;
            unit[idx].positionY = y;
            idx++;
            if (idx == battalion_size) return;
            
        }

    }

}