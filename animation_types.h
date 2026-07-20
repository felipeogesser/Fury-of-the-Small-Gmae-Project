#ifndef ANIMATION_TYPES_H
#define ANIMATION_TYPES_H

typedef struct Animation Animation;
typedef struct AnimationState AnimationState;

enum SpriteState {
    IDLE,
    RUN,
    ATTACK,
    GUARD
};

#endif