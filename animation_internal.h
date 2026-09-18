#ifndef ANIMATION_INTERNAL_H
#define ANIMATION_INTERNAL_H

#include "animation_types.h"

typedef struct AnimationState {

    enum Animation animation;
    unsigned char frames_count;
    unsigned char current_frame;

} AnimationState;

#endif