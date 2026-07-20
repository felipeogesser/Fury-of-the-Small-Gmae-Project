#ifndef ENTITIES_INTERNAL_H
#define ENTITIES_INTERNAL_H

#include <SDL2/SDL.h>

//#define MAX_ENTITIES 1024

typedef struct Entity {
    unsigned int id;
    char name[32];
    char team[32];
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
    unsigned char *sprite_state;
} Entity;

//extern Entity *entities;

#endif