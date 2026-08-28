#include "general.h"
#include "general_internal.h"
#include "animation_types.h"
#include "armies_internal.h"
#include "battalion_internal.h"
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

// private prototypes
static void set_general_sprite_width_and_height(General *general);
static void copy_bits_set_to_one(unsigned char *d, const unsigned char *s);
static void set_general_xy_position(Battalion *battalion, General *general);
static void set_general_dimension(General *general);

void init_generals(Battalion *battalion, General *general) {
    
    copy_bits_set_to_one((unsigned char *)battalion->general, (unsigned char *)general);

    set_general_sprite_width_and_height(general);

    set_general_xy_position(battalion, general);

    set_general_dimension(general);

}

void update_generals(Armies *armies, GameState *game) {
    
    General *general = armies->army->general;
    unsigned int general_count = armies->army->general_count;
    for (unsigned int i = 0; i < general_count; i++) {
        general[i].positionX += general[i].vectorX * game->delta;
        general[i].positionY += general[i].vectorY * game->delta;
    }

}

static void copy_bits_set_to_one(unsigned char *d, const unsigned char *s) {

    // d's bytes that are non-zero should always be zero in s.
    // if this stops being true, than funtion needs to change.
    for (size_t i = 0; i < sizeof(General); i++) {
        d[i] |= s[i];
    }

}

static void set_general_sprite_width_and_height(General *general) {

    general->sprite.w = engine.sprite_pack->sprite[general->sprite.type][general->anim.animation].width / general->anim.frames_count;
    general->sprite.h = engine.sprite_pack->sprite[general->sprite.type][general->anim.animation].height;

}

static void set_general_xy_position(Battalion *battalion, General *general) {

    general->positionX = battalion->initial_map_placement_x + battalion->area_width - 5;
    general->positionY = battalion->initial_map_placement_y + battalion->area_height / 2;

}

static void set_general_dimension(General *general) {

    general->dimensionX = general->sprite.w;
    general->dimensionY = general->sprite.h;

}
