#ifndef MAPS_H
#define MAPS_H

#include "maps_types.h"

unsigned int create_map(signed short mapSizeX, signed short mapSizeY);

Map *get_map(unsigned int mapId);

#endif