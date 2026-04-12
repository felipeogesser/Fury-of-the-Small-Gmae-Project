#ifndef ENTITIES
#define ENTITIES

#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define MAX_ENTITIES 1024

typedef struct Entity {
    int id;
    char name[32];
    float positionX, positionY;
    float dimensionX, dimensionY;
    int R_Color;
    int G_Color;
    int B_Color;
    int Alpha;
    bool collision;
    int quadrant;
    float speed;
    float vectorX, vectorY;
    float randomX, randomY;
    float directionX, directionY;
    float hypotenuseLengh;
    Uint32 lastTick;
    int nextMoveDelay;

} Entity;

extern Entity *entities;

int create_entity(const char *name, float positionX, float positionY,
    float dimensionX, float dimensionY,
    int R_Color, int G_Color, int B_Color, int Alpha,
    bool collision, int speed);
Entity *get_entity(int id);

void init_entities(void);

#endif