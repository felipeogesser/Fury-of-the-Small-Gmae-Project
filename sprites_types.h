#ifndef SPRITES_TYPES_H
#define SPRITES_TYPES_H

#include "field_entry_internal.h"

typedef struct Sprite Sprite;
typedef struct SpritePack SpritePack;
typedef struct SpriteInfo SpriteInfo;

enum Sprites {
    SPRITE_UNKNOWN = -1,
    SPRITE_ARCHERS,
    SPRITE_CAVALRY,
    SPRITE_GUARDSMEN,
    SPRITE_SWORDSMEN,
    SPRITE_TEMPLAR,
    SPRITE_GENERAL,
    SPRITES_COUNT
};

extern const ComplexFieldEntry *const sprite_complex_field_table_ptr;
extern const ComplexFieldEntry sprite_info_complex_field_table[];
extern const ComplexFieldEntry *const sprite_pack_complex_field_table_ptr;

#endif