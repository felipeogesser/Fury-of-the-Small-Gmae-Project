#ifndef MAPS_INTERNAL_H
#define MAPS_INTERNAL_H

#define MAX_MAPS 10

typedef struct Map {
    unsigned int id;
    signed short mapSizeX, mapSizeY;
    signed short mapLeftLimit, mapRightLimit, mapTopLimit, mapBottomLimit;
} Map;

#endif