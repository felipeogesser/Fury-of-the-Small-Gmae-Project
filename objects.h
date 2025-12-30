#pragma once
#include <stdbool.h>

#define MAX_OBJECTS 128

typedef struct Object {
    int id;
    char name[32];
    float pointX;
    float pointY;
    float dimensionX;
    float dimensionY;
    bool collision;
    int quadrant;
} Object;

int create_object(const char *name, float pointX, float pointY,
    float dimensionX, float dimensionY, bool collision, int quadrant);
 Object *get_object(int id);