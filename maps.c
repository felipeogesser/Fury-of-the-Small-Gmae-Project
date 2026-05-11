#include "maps.h"

static Map maps[MAX_MAPS];
static int map_count = 0;
static int mapIndex;
int create_map(float mapSizeX, float mapSizeY) {
    if (map_count >= MAX_MAPS) return -1;
    mapIndex = map_count;
    maps[mapIndex].id = mapIndex + 1;
    maps[mapIndex].mapSizeX = mapSizeX;
    maps[mapIndex].mapSizeY = mapSizeY;
    maps[mapIndex].mapLeftLimit = 0;
    maps[mapIndex].mapRightLimit = mapSizeX;
    maps[mapIndex].mapTopLimit = 0;
    maps[mapIndex].mapBottomLimit = mapSizeY;
    map_count++;
    return maps[mapIndex].id;
}

Map *get_map(int mapIndex) {
    if (mapIndex <= 0 || mapIndex > map_count) return NULL;
    return &maps[mapIndex - 1];
}