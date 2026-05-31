#ifndef MAPS
#define MAPS

#define MAX_MAPS 10

typedef struct Map {
    unsigned int id;
    signed short mapSizeX, mapSizeY;
    signed short mapLeftLimit, mapRightLimit, mapTopLimit, mapBottomLimit;
} Map;

unsigned int create_map(signed short mapSizeX, signed short mapSizeY);

Map *get_map(unsigned int mapId);

#endif