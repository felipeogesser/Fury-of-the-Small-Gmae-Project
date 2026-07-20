#ifndef ANIMATION_INTERNAL_H
#define ANIMATION_INTERNAL_H

#include "animation_types.h"
#include <SDL2/SDL.h>

typedef struct Animation {
    char *animation_path;

} Animation;

typedef struct AnimationState {
    //SDL_Surface *surface;
    //SDL_Texture *texture;
    //Animation current;
    enum SpriteState sprite_state;
    unsigned int frame;
    unsigned int frame_count;
    float timer;
    float frame_duration;
    unsigned int frame_w;
    unsigned int frame_h;
    unsigned char sprite_frame;
} AnimationState;

#endif