#ifndef ANIMATION_TYPES_H
#define ANIMATION_TYPES_H

typedef struct AnimationState AnimationState;

enum Animation {
    ANIMATION_UNKNOWN = -1,
    IDLE = 0,
    RUN,
    ATTACK,
    GUARD,
    ANIMATION_COUNT
};

#endif