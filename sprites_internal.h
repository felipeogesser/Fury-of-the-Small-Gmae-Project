#ifndef SPRITES__INTERNAL_H
#define SPRITES__INTERNAL_H

#include "sprites_types.h"
#include "animation_types.h"
#include <SDL2/SDL.h>

typedef struct Sprite {
    SDL_Texture *texture;
    unsigned char frames_count;    
    signed short width, height;
} Sprite;

typedef struct SpriteInfo {

    enum Sprites type;
    //unsigned char sprite_frames_count;
    //unsigned char sprite_current_frame;

}SpriteInfo;

typedef struct SpritePack {
    Sprite sprite[SPRITES_COUNT][ANIMATION_COUNT];
    unsigned int sprite_count;
} SpritePack;

//extern SpritePack sprite_pack;

#endif