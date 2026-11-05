#ifndef ANIMATION_TYPES_H
#define ANIMATION_TYPES_H

#include "field_entry_internal.h"

typedef struct AnimationState AnimationState;

enum Animation {
    ANIMATION_UNKNOWN = -1,
    IDLE = 0,
    WALK,
    RUN,
    ATTACK_01,
    ATTACK_02,
    GUARD,
    DEATH,
    ANIMATION_COUNT
};

extern const ComplexFieldEntry animation_state_complex_field_table[];

#endif