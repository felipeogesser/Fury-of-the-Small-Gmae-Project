#ifndef SPRITES__INTERNAL_H
#define SPRITES__INTERNAL_H

#include "sprites_types.h"
#include <SDL2/SDL.h>

typedef struct Sprite {
    unsigned int frames_count;
    SDL_Texture *texture;
} Sprite;

typedef struct SpritePack {
    Sprite *sprite;
    unsigned int sprite_count;
} SpritePack;

#endif