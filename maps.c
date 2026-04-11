#include "maps.h"

static Map maps[MAX_MAPS];
static int map_count;

int create_map(float mapSizeX, float mapSizeY) {
    if (map_count >= MAX_MAPS) return -1;
    int id = map_count;
    maps[id].id = id;
    maps[id].mapSizeX = mapSizeX;
    maps[id].mapSizeY = mapSizeY;
    maps[id].mapLeftLimit = 0;
    maps[id].mapRightLimit = mapSizeX;
    maps[id].mapTopLimit = 0;
    maps[id].mapBottomLimit = mapSizeY;
    map_count++;
    return id;
}

Map *get_map(int id) {
    if (id < 0 || id >= map_count) return NULL;
    return &maps[id];
}