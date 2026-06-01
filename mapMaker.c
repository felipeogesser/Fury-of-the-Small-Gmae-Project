#include "mapMaker.h"
#include "maps.h"

unsigned int map_id[MAX_MAPS];

void make_maps(void) {
    map_id[0] = create_map(1024, 1024);
}