#ifndef ANIMATION_TYPES_H
#define ANIMATION_TYPES_H

typedef struct AnimationState AnimationState;

enum Animation {
    IDLE = 0,
    RUN,
    ATTACK,
    GUARD,
    ANIMATION_COUNT
};

#endif