#include "unit.h"
#include "unit_internal.h"
#include "animation_types.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "general_internal.h"
#include "sprites_internal.h"


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


void create_unit(Unit *unit, General *general, float ongoing_point_X, float ongoing_point_Y, unsigned int i, unsigned int k) {
    unit[i].id = i + 1;
    unit[i].positionX = ongoing_point_X;
    unit[i].positionY = ongoing_point_Y;
    unit[i].dimensionX = 4;
    unit[i].dimensionY = 4;
    unit[i].anim.animation = IDLE;
    unit[i].anim.frames_count = engine.sprite_pack->sprite[general[k].units_type][IDLE].frames_count;
    unit[i].anim.current_frame = (unsigned char)(rand() % unit[i].anim.frames_count);
    unit[i].sprite.type = general[k].units_type;
}

void update_units(Armies *armies, GameState *game) {
    
    Unit *unit = armies->army->general->battalions->unit;

    for (unsigned int i = 0; i < game->unit_created_count; i++) {
        unit[i].positionX += unit[i].vectorX * game->delta;
        unit[i].positionY += unit[i].vectorY * game->delta;
    }

}





/*

tem que add feature na unti pra ela saber qual e a sprite dela,
referenciar com ponterio pra quando rocar de animacao saber quantos frames



*/