#ifndef MAPS
#define MAPS

#define MAX_MAPS 10

typedef struct Map {
    int id;
    float MapSizeX, MapSizeY;
} Map;

int create_map(float MapSizeX, float MapSizeY);

Map *get_map(int id);
#endif