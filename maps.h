#ifndef MAPS
#define MAPS

#include <stdio.h>

#define MAX_MAPS 10

typedef struct Map {
    int id;
    float mapSizeX, mapSizeY;
    float mapLeftLimit, mapRightLimit, mapTopLimit, mapBottomLimit;
} Map;

int create_map(float mapSizeX, float mapSizeY);

Map *get_map(int id);

#endif