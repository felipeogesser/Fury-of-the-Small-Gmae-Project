#include "general.h"
#include "general_internal.h"
#include "animation_types.h"
#include "armies_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "sprites_types.h"

#define BATTALION_LIST(X) \
    X(INFANTRY,  MELEE) \
    X(CAVALRY,   MELEE) \
    X(SPEARMEN,  MELEE) \
    X(ARCHERS,   RANGED)

static unsigned int internal_taxonomy_masks[GENERAL_TYPE_COUNT];

const unsigned int *const general_battalion_taxonomy_type_masks = internal_taxonomy_masks;

void init_general_battalion_taxonomy(void) {
    #define X(name, group) internal_taxonomy_masks[group] |= (1U << name);
        BATTALION_LIST(X)
    #undef X
}

#define OFFSET_OF(type, member) ((size_t) &(((type *)0)->member))
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))

const FieldEntry general_field_table[] = {
    { "anim",            OFFSET_OF(General, anim),            SIZE_OF(General, anim) },
    { "sprite",          OFFSET_OF(General, sprite),          SIZE_OF(General, sprite) },
    { "id",              OFFSET_OF(General, id),              SIZE_OF(General, id) },
    { "rarity",          OFFSET_OF(General, rarity),          SIZE_OF(General, rarity) },
    { "hp",              OFFSET_OF(General, hp),              SIZE_OF(General, hp) },
    { "vigour",          OFFSET_OF(General, vigour),          SIZE_OF(General, vigour) },
    { "attack",          OFFSET_OF(General, attack),          SIZE_OF(General, attack) },
    { "defense",         OFFSET_OF(General, defense),         SIZE_OF(General, defense) },
    { "evasion",         OFFSET_OF(General, evasion),         SIZE_OF(General, evasion) },
    { "attack_speed",    OFFSET_OF(General, attack_speed),    SIZE_OF(General, attack_speed) },
    { "general_type",    OFFSET_OF(General, general_type),    SIZE_OF(General, general_type) },
    { "battalion_type",  OFFSET_OF(General, battalion_type),  SIZE_OF(General, battalion_type) },
    { "units_type",      OFFSET_OF(General, units_type),      SIZE_OF(General, units_type) },
    { "obj_size",        0,                                   sizeof(General) },
};
const size_t general_field_table_count =
    sizeof(general_field_table) / sizeof(general_field_table[0]);
#undef OFFSET_OF
#undef SIZE_OF

void create_generals(General *general, float ongoing_point_X, float ongoing_point_Y, unsigned int i) {
    general[i].id = i + 1;
    general[i].positionX = ongoing_point_X + 40;
    general[i].positionY = ongoing_point_Y + 60;
    general[i].dimensionX = 8;
    general[i].dimensionY = 8;
    general[i].units_type = WARRIOR;
    general[i].sprite.type = WARRIOR;
    general[i].anim.animation = IDLE;
    engine.game->generals_created_count++;
}

void update_generals(Armies *armies, GameState *game) {
    
    General *general = armies->army->general;

    for (unsigned int i = 0; i < 6; i++) {
        general[i].positionX += general[i].vectorX * game->delta;
        general[i].positionY += general[i].vectorY * game->delta;
    }

}