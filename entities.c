#include "entities.h"
#include "entities_internal.h"
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
    { "anim",            OFFSET_OF(Entity, anim),            SIZE_OF(Entity, anim) },
    { "sprite",          OFFSET_OF(Entity, sprite),          SIZE_OF(Entity, sprite) },
    { "id",              OFFSET_OF(Entity, id),              SIZE_OF(Entity, id) },
    { "SENTINEL_VALUE",  sizeof(Entity),                     0 },
};
const size_t unit_field_table_count =
    sizeof(unit_field_table) / sizeof(unit_field_table[0]);
#undef OFFSET_OF
#undef SIZE_OF


void create_entities(Entity *entities, General *general, float ongoing_point_X, float ongoing_point_Y, unsigned int i, unsigned int k) {
    entities[i].id = i + 1;
    entities[i].positionX = ongoing_point_X;
    entities[i].positionY = ongoing_point_Y;
    entities[i].dimensionX = 4;
    entities[i].dimensionY = 4;
    entities[i].anim.animation = IDLE;
    entities[i].anim.frames_count = engine.sprite_pack->sprite[general[k].units_type][IDLE].frames_count;
    entities[i].anim.current_frame = (unsigned char)(rand() % entities[i].anim.frames_count);
    entities[i].sprite.type = general[k].units_type;
}

void update_units(Armies *armies, GameState *game) {
    
    Entity *entities = armies->army->general->battalions->entities;

    for (unsigned int i = 0; i < game->entities_created_count; i++) {
        entities[i].positionX += entities[i].vectorX * game->delta;
        entities[i].positionY += entities[i].vectorY * game->delta;
    }

}





/*

tem que add feature na unti pra ela saber qual e a sprite dela,
referenciar com ponterio pra quando rocar de animacao saber quantos frames



*/