#include "maps.h"
#include "maps_internal.h"
#include <stddef.h>

static Map maps[MAX_MAPS];
static unsigned int map_count = 0;
static unsigned int mapIndex;

unsigned int create_map(signed short mapSizeX, signed short mapSizeY) {
    if (map_count >= MAX_MAPS) return 1;
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

Map *get_map(unsigned int mapId) {
    if (mapId <= 0 || mapId > map_count) return NULL;
    return &maps[mapId - 1];
}