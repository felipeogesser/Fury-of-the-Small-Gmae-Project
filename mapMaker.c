#include "mapMaker.h"
#include "maps.h"

unsigned int map_id[MAX_MAPS];

#define MAX_ENTITY_PIXEL_HEIGHT 48
#define MAX_BATTALION_FORMATION_HEIGHT 10
#define MAX_BATTLEPLAN_GRID_HEIGHT 7



void make_maps(void) {

    unsigned int height = MAX_ENTITY_PIXEL_HEIGHT * MAX_BATTALION_FORMATION_HEIGHT * MAX_BATTLEPLAN_GRID_HEIGHT;
    unsigned int width = height * 2;
    map_id[0] = create_map(width, height);
}