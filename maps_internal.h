#ifndef MAPS_INTERNAL_H
#define MAPS_INTERNAL_H

#include "maps_types.h"

typedef struct Padding {

    unsigned int left;
    unsigned int right;
    unsigned int top;
    unsigned int bottom;
    unsigned int in_between_armies;

} Padding;

typedef struct Map {

    Padding padding;
    unsigned short mapSizeX, mapSizeY;
    unsigned short mapLeftLimit, mapRightLimit, mapTopLimit, mapBottomLimit;

} Map;

#endif