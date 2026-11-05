#include "general.h"
#include "general_internal.h"
#include "animation_internal.h"
#include "armies_internal.h"
#include "engine_internal.h"
#include "field_entry_internal.h"
#include "game_state_internal.h"
#include "maps_internal.h"
#include "sprites_internal.h"
#include <stddef.h>
/*
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
}*/
#define OFFSET_OF(type, member) offsetof(type, member)
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))

static const FieldEntry field_table[] = {

    { "animation",               OFFSET_OF(General, anim),               SIZE_OF(General, anim)               },
    { "sprite",             OFFSET_OF(General, sprite),             SIZE_OF(General, sprite)             },
    { "sprite_type",             OFFSET_OF(General, sprite_type),             SIZE_OF(General, sprite_type)             },
    { "direction",             OFFSET_OF(General, direction),             SIZE_OF(General, direction)             },
    { "id",                 OFFSET_OF(General, id),                 SIZE_OF(General, id)                 },
    { "rarity",             OFFSET_OF(General, rarity),             SIZE_OF(General, rarity)             },
    { "drawer_slot_index",  OFFSET_OF(General, drawer_slot_index),  SIZE_OF(General, drawer_slot_index)  },
    { "render",             OFFSET_OF(General, render),             SIZE_OF(General, render)             },
    { "hp",                 OFFSET_OF(General, hp),                 SIZE_OF(General, hp)                 },
    { "vigour",             OFFSET_OF(General, vigour),             SIZE_OF(General, vigour)             },
    { "base_meele_attack",  OFFSET_OF(General, base_meele_attack),  SIZE_OF(General, base_meele_attack)  },
    { "base_meele_defense", OFFSET_OF(General, base_meele_defense), SIZE_OF(General, base_meele_defense) },
    { "armor",              OFFSET_OF(General, armor),              SIZE_OF(General, armor)              },
    { "base_strength",           OFFSET_OF(General, base_strength),           SIZE_OF(General, base_strength)           },
    { "attack_speed",       OFFSET_OF(General, attack_speed),       SIZE_OF(General, attack_speed)       },
    { "positionX",          OFFSET_OF(General, positionX),          SIZE_OF(General, positionX)          },
    { "positionY",          OFFSET_OF(General, positionY),          SIZE_OF(General, positionY)          },
    { "dimensionX",         OFFSET_OF(General, dimensionX),         SIZE_OF(General, dimensionX)         },
    { "dimensionY",         OFFSET_OF(General, dimensionY),         SIZE_OF(General, dimensionY)         },
    { "speed",              OFFSET_OF(General, speed),              SIZE_OF(General, speed)              },
    { "vectorX",            OFFSET_OF(General, vectorX),            SIZE_OF(General, vectorX)            },
    { "vectorY",            OFFSET_OF(General, vectorY),            SIZE_OF(General, vectorY)            },
    { "R_color",            OFFSET_OF(General, R_color),            SIZE_OF(General, R_color)            },
    { "G_color",            OFFSET_OF(General, G_color),            SIZE_OF(General, G_color)            },
    { "B_color",            OFFSET_OF(General, B_color),            SIZE_OF(General, B_color)            },
    { "Alpha",              OFFSET_OF(General, Alpha),              SIZE_OF(General, Alpha)              },
    { "name",               OFFSET_OF(General, name),               SIZE_OF(General, name)               },
    { "general_type",       OFFSET_OF(General, general_type),       SIZE_OF(General, general_type)       },
    { "battalion_type",     OFFSET_OF(General, battalion_type),     SIZE_OF(General, battalion_type)     },
    { "battalions",         OFFSET_OF(General, battalions),         SIZE_OF(General, battalions)         },
    { "units_type",         OFFSET_OF(General, units_type),         SIZE_OF(General, units_type)         },
    { "range",              OFFSET_OF(General, range),              SIZE_OF(General, range)              },
    { "charge_bonus",       OFFSET_OF(General, charge_bonus),       SIZE_OF(General, charge_bonus)       },
    { "is_shielded",           OFFSET_OF(General, is_shielded),           SIZE_OF(General, is_shielded)           },
    { "is_ranged",             OFFSET_OF(General, is_ranged),             SIZE_OF(General, is_ranged)             },
    { "is_large",              OFFSET_OF(General, is_large),              SIZE_OF(General, is_large)              },
    { "is_armor_piercing",     OFFSET_OF(General, is_armor_piercing),     SIZE_OF(General, is_armor_piercing)     },
    { "is_anti_large",         OFFSET_OF(General, is_anti_large),         SIZE_OF(General, is_anti_large)         },
    { "obj_size", 0, sizeof(General) }

};
const FieldEntry *const general_field_table = field_table;

static const ComplexFieldEntry general_complex_field_table[] = {

    { "animation",               OFFSET_OF(General, anim),               SIZE_OF(General, anim),               STRUCT,      animation_state_complex_field_table },
    { "sprite",             OFFSET_OF(General, sprite),             SIZE_OF(General, sprite),             STRUCT,      sprite_info_complex_field_table },
    { "sprite_type",             OFFSET_OF(General, sprite_type),             SIZE_OF(General, sprite_type),             S_INT,      NULL },
    { "direction",             OFFSET_OF(General, direction),             SIZE_OF(General, direction),             S_INT,      NULL },
    { "id",                 OFFSET_OF(General, id),                 SIZE_OF(General, id),                 U_INT,       NULL },
    { "rarity",             OFFSET_OF(General, rarity),             SIZE_OF(General, rarity),             S_INT,       NULL },
    { "drawer_slot_index",  OFFSET_OF(General, drawer_slot_index),  SIZE_OF(General, drawer_slot_index),  U_CHAR,      NULL },
    { "render",             OFFSET_OF(General, render),             SIZE_OF(General, render),             BOOL,        NULL },
    { "hp",                 OFFSET_OF(General, hp),                 SIZE_OF(General, hp),                 S_INT,       NULL },
    { "vigour",             OFFSET_OF(General, vigour),             SIZE_OF(General, vigour),             S_INT,       NULL },
    { "base_meele_attack",  OFFSET_OF(General, base_meele_attack),  SIZE_OF(General, base_meele_attack),  S_INT,       NULL },
    { "base_meele_defense", OFFSET_OF(General, base_meele_defense), SIZE_OF(General, base_meele_defense), S_INT,       NULL },
    { "armor",              OFFSET_OF(General, armor),              SIZE_OF(General, armor),              S_INT,       NULL },
    { "base_strength",           OFFSET_OF(General, base_strength),           SIZE_OF(General, base_strength),           FLOAT,       NULL },
    { "attack_speed",       OFFSET_OF(General, attack_speed),       SIZE_OF(General, attack_speed),       FLOAT,       NULL },
    { "x",          OFFSET_OF(General, positionX),          SIZE_OF(General, positionX),          FLOAT,       NULL },
    { "y",          OFFSET_OF(General, positionY),          SIZE_OF(General, positionY),          FLOAT,       NULL },
    { "dimensionX",         OFFSET_OF(General, dimensionX),         SIZE_OF(General, dimensionX),         FLOAT,       NULL },
    { "dimensionY",         OFFSET_OF(General, dimensionY),         SIZE_OF(General, dimensionY),         FLOAT,       NULL },
    { "speed",              OFFSET_OF(General, speed),              SIZE_OF(General, speed),              FLOAT,       NULL },
    { "vectorX",            OFFSET_OF(General, vectorX),            SIZE_OF(General, vectorX),            FLOAT,       NULL },
    { "vectorY",            OFFSET_OF(General, vectorY),            SIZE_OF(General, vectorY),            FLOAT,       NULL },
    { "R_color",            OFFSET_OF(General, R_color),            SIZE_OF(General, R_color),            U_CHAR,      NULL },
    { "G_color",            OFFSET_OF(General, G_color),            SIZE_OF(General, G_color),            U_CHAR,      NULL },
    { "B_color",            OFFSET_OF(General, B_color),            SIZE_OF(General, B_color),            U_CHAR,      NULL },
    { "Alpha",              OFFSET_OF(General, Alpha),              SIZE_OF(General, Alpha),              U_CHAR,      NULL },
    { "name",               OFFSET_OF(General, name),               SIZE_OF(General, name),               S_CHAR,      NULL },
    { "general_type",       OFFSET_OF(General, general_type),       SIZE_OF(General, general_type),       S_INT,       NULL },
    { "battalion_type",     OFFSET_OF(General, battalion_type),     SIZE_OF(General, battalion_type),     S_INT,       NULL },
    { "battalions",         OFFSET_OF(General, battalions),         SIZE_OF(General, battalions),         NOT_DEFINED, NULL },
    { "units_type",         OFFSET_OF(General, units_type),         SIZE_OF(General, units_type),         S_INT,       NULL },
    { "range",              OFFSET_OF(General, range),              SIZE_OF(General, range),              FLOAT,       NULL },
    { "charge_bonus",       OFFSET_OF(General, charge_bonus),       SIZE_OF(General, charge_bonus),       FLOAT,       NULL },
    { "is_shielded",           OFFSET_OF(General, is_shielded),           SIZE_OF(General, is_shielded),           BOOL,        NULL },
    { "is_ranged",             OFFSET_OF(General, is_ranged),             SIZE_OF(General, is_ranged),             BOOL,        NULL },
    { "is_large",              OFFSET_OF(General, is_large),              SIZE_OF(General, is_large),              BOOL,        NULL },
    { "is_armor_piercing",     OFFSET_OF(General, is_armor_piercing),     SIZE_OF(General, is_armor_piercing),     BOOL,        NULL },
    { "is_anti_large",         OFFSET_OF(General, is_anti_large),         SIZE_OF(General, is_anti_large),         BOOL,        NULL },
    { "obj_size", 0, sizeof(General), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const general_complex_field_table_ptr = general_complex_field_table;

#undef OFFSET_OF
#undef SIZE_OF

const size_t sizeof_General = sizeof(General);

// private prototypes
static void copy_bits_set_to_one(unsigned char *d, const unsigned char *s);
static void set_general_sprite_width_and_height(General *general);
static void set_general_dimension(General *general);
static void set_general_position(General *general, unsigned int x, unsigned int y);

void init_general(General *general, General *general_from_battleplan_payload, unsigned int x, unsigned int y) {
    
    copy_bits_set_to_one((unsigned char *)general, (unsigned char *)general_from_battleplan_payload);

    set_general_sprite_width_and_height(general);

    set_general_dimension(general);

    set_general_position(general, x, y);

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

static void set_general_dimension(General *general) {

    general->dimensionX = general->sprite.w;
    general->dimensionY = general->sprite.h;

}

static void set_general_position(General *general, unsigned int x, unsigned int y) {

    general->positionX = x;
    general->positionY = y;

}
