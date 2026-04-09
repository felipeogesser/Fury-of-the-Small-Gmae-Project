#include "maps.h"

static Map maps[MAX_MAPS];
static int map_count;

int create_map(float MapSizeX, float MapSizeY) {
    if (map_count >= MAX_MAPS) return -1;
    int id = map_count;
    maps[id].id = id;
    maps[id].MapSizeX = MapSizeX;
    maps[id].MapSizeY = MapSizeY;
    map_count++;
    return id;
}

Map *get_map(int id) {
    if (id < 0 || id >= map_count) return NULL;
    return &maps[id];
}