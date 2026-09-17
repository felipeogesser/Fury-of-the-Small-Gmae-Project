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

static const FieldEntry field_table[] = {

    #define OFFSET_OF(type, member) ((size_t) &(((type *)0)->member))
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "anim",            OFFSET_OF(Unit, anim),            SIZE_OF(Unit, anim) },
    { "sprite",          OFFSET_OF(Unit, sprite),          SIZE_OF(Unit, sprite) },
    { "id",              OFFSET_OF(Unit, id),              SIZE_OF(Unit, id) },
    { "obj_size",        0,                                sizeof(Unit) },

    #undef OFFSET_OF
    #undef SIZE_OF

};
//const size_t unit_field_table_count =
//    sizeof(unit_field_table) / sizeof(unit_field_table[0]);
const FieldEntry *const unit_field_table = field_table;


// private prototypes
void update_units_position(Unit *unit, unsigned int unit_count, signed int x, signed int y);

static void set_units_id(Unit *unit, unsigned int battalion_size);
static void set_units_dimension(Unit *unit, unsigned int battalion_size);
static void set_units_sprite_and_animation(Unit *unit, General *general, unsigned int battalion_size);
static void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion, unsigned int x, unsigned int y);


void init_units(Battalion *battalion, unsigned int x, unsigned int y) {
    // this function expects to be called only when generals are already initialized
    General *general = battalion->general;
    Unit *unit = battalion->unit;
    unsigned int battalion_size = battalion->unit_count;

    set_units_id(unit, battalion_size);
    set_units_sprite_and_animation(unit, general, battalion_size);
    set_units_dimension(unit, battalion_size);
    set_units_position(unit, battalion_size, battalion, x, y);

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

///
//usar o general pra definir pos das units, passar genral como arg
//////
static void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion, unsigned int x, unsigned int y) {

    unsigned int formation_height = battalion->unit_formation_height;
    unsigned int formation_width = ceilf(battalion_size / battalion->unit_formation_height);

    unsigned int pad_x = battalion->padding_between_units_x;
    unsigned int pad_y = battalion->padding_between_units_y;

    unsigned int pos_x = x;
    unsigned int pos_y = y;
    unsigned int dim_x = unit->dimensionX;
    unsigned int dim_y = unit->dimensionY;
    if (battalion_size < formation_height) {

        pos_x -= dim_x * 2;
        pos_y -= dim_y / 2 - (dim_y + pad_y) * battalion_size / 2;
        
    } else {

        pos_x -= dim_x * 2;
        pos_y += dim_y / 2 - (dim_y + pad_y) * formation_height / 2;
        
    }

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

/*void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion, BattleplanGrid *grid, OccupiedCell *occupied_cell) {

    unsigned int formation_height = battalion->unit_formation_height;
    unsigned int formation_width = ceilf(battalion_size / battalion->unit_formation_height);

    unsigned int unit_padding_x = battalion->padding_between_units_x;
    unsigned int unit_padding_y = battalion->padding_between_units_y;

    unsigned int cell_width = (engine.map->mapSizeX / 2 - engine.map->padding.in_between_armies / 2 - engine.map->padding.left) / grid->dimension.x;
    unsigned int cell_height = (engine.map->mapSizeY - engine.map->padding.bottom - engine.map->padding.top ) / grid->dimension.y;

    unsigned int cell_position_x = engine.map->padding.left + occupied_cell->x * cell_width;
    unsigned int cell_position_y = engine.map->padding.top + occupied_cell->y * cell_height;

    unsigned int cell_inner_padding_top = 0;
    if (battalion_size < formation_height) {

        cell_inner_padding_top = ((cell_height / (unit->sprite.h + unit_padding_y)) * (formation_height - battalion_size)) / 2;

    } else {

        cell_inner_padding_top = (cell_height - formation_height * (unit->sprite.h + unit_padding_y)) / 2;

    }

    unsigned int cell_inner_padding_left = 0;
    cell_inner_padding_left = cell_width / 2 - formation_width * (unit->sprite.w + unit_padding_x);


    unsigned int idx = 0;
    for (unsigned int i = 0; i < formation_width; i++) {

        for (unsigned int j = 0; j < formation_height; j++) {

            unsigned int x = cell_position_x + cell_inner_padding_left + unit_padding_x * i + (unit[idx].dimensionX * i);
            unsigned int y = cell_position_y + cell_inner_padding_top + unit_padding_y * j + (unit[idx].dimensionY * j);
            unit[idx].positionX = x;
            unit[idx].positionY = y;
            idx++;
            if (idx == battalion_size) return;
            
        }

    }

}*/