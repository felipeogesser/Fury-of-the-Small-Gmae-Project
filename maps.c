#include "maps.h"
#include "maps_types.h"
#include "maps_internal.h"
#include <stdio.h>
#include <stdlib.h>

Map map = {0};

Map *map_init(unsigned int map_size_x, unsigned int map_size_y) {


    unsigned short max_u_short_value = -1;
    if (map_size_x > max_u_short_value || map_size_y > max_u_short_value) {

        fprintf(stderr, "values used to init map size exceed maximum size");
        exit(EXIT_FAILURE);

    }
    map.mapSizeX = map_size_x;
    map.mapSizeY = map_size_y;
    map.mapLeftLimit = 0;
    map.mapRightLimit = map_size_x;
    map.mapTopLimit = 0;
    map.mapBottomLimit = map_size_y;

    return &map;

}

/*Map *get_map(unsigned int mapId) {
    if (mapId <= 0 || mapId > map_count) return NULL;
    return &maps[mapId - 1];
}*/