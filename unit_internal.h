#ifndef UNIT_INTERNAL_H
#define UNIT_INTERNAL_H

#include "unit_types.h"
#include "animation_internal.h"
#include "direction_types.h"
#include "sprites_internal.h"
#include "target_types.h"
#include <SDL2/SDL.h>

typedef struct Unit {
    AnimationState anim;
    SpriteInfo sprite;
    unsigned int id;
    char name[32];
    float positionX, positionY;
    float dimensionX, dimensionY;
    float quadrantOccupiedX, quadrantOccupiedY;
    int previousCol, previousRow;
    int previousX, previousY;
    _Bool collision;
    float speed;
    float vectorX, vectorY;
    Uint32 lastTick;
    int nextMoveDelay;
    int currentQuadrants[4];
    unsigned char quadrantOutOfBounds;
    int enemyUnitId;
    _Bool pathFound;
    float constMovX, constMovY;
    float health;

    enum Direction direction;
    unsigned int target_idx;
    enum TargetType target_type;
    
} Unit;

#endif