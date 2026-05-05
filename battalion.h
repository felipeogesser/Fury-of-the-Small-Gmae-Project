#ifndef BATTALION_H
#define BATTALION_H

#include "entities.h"

typedef struct Battalion {
    struct Entity *entities;
    unsigned int entities_count;
    unsigned char R_Color;
    unsigned char G_Color;
    unsigned char B_Color;
    unsigned char Alpha;
} Battalion;

#ifndef