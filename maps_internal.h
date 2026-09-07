#ifndef MAPS_INTERNAL_H
#define MAPS_INTERNAL_H

#include "maps_types.h"

typedef struct Map {

    unsigned short mapSizeX, mapSizeY;
    unsigned short mapLeftLimit, mapRightLimit, mapTopLimit, mapBottomLimit;

} Map;

#endif