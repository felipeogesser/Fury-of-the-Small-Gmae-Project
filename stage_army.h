#ifndef STAGE_ARMY_H
#define STAGE_ARMY_H

#include "battleplan_grid_types.h"
#include "general_types.h"
#include <stdint.h>

#define STAGE_ARMY_BASE_SIZE 7u
#define STAGE_ARMY_MAX_SIZE 20u

unsigned int stage_army_size(unsigned int stage);
void stage_army_layout(unsigned int stage, uint64_t seed, unsigned int (*layout)[GRID_DIMENSION_Y]);
enum GeneralType stage_army_loot(unsigned int stage, uint64_t seed);

#endif
