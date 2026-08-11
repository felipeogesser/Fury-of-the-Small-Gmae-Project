#ifndef ANIMATION_INTERNAL_H
#define ANIMATION_INTERNAL_H

#include "animation_types.h"
#include <SDL2/SDL.h>

/*typedef struct Animation {
    char *animation_path;

} Animation;*/

typedef struct AnimationState {
    //SDL_Surface *surface;
    //SDL_Texture *texture;
    enum Animation animation;
    unsigned char frames_count;
    unsigned char current_frame;


    /*unsigned int frame;
    unsigned int frame_count;
    float timer;
    float frame_duration;
    unsigned int frame_w;
    unsigned int frame_h;
    unsigned char sprite_frame;
    unsigned char unit_sprite_state[8];*/
} AnimationState;

#endif