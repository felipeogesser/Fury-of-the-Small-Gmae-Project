#include "map_maker.h"
#include "maps.h"

int map_id[MAX_MAPS];
void make_maps(void) {
    map_id[0] = create_map(800, 600);
}