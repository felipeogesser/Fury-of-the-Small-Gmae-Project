#ifndef ENTITIES_INTERNAL_H
#define ENTITIES_INTERNAL_H

#include "animation_internal.h"
#include "field_entry.h"
#include "sprites_internal.h"
#include <SDL2/SDL.h>

//#define MAX_ENTITIES 1024

extern const FieldEntry unit_field_table[];
extern const size_t unit_field_table_count;

typedef struct Entity {
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
    //unsigned char alreadyInQuadrant;
    unsigned char quadrantOutOfBounds;
    int enemyEntityId;
    _Bool pathFound;
    float constMovX, constMovY;
    float health;
    //unsigned char *sprite_current_frame;
    //unsigned char sprite_frames_count;
    //unsigned char sprite_current_frame;
    //enum Animation animation;
    //enum Sprites unit_type;
    //Sprite sprite;
} Entity;

//extern Entity *entities;

#endif