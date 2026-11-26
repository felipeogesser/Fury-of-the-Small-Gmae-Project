#ifndef STAGE_ARMY_H
#define STAGE_ARMY_H

#include "battleplan_grid_types.h"
#include "general_types.h"
#include <stdint.h>

#define STAGE_ARMY_BASE_SIZE 7u
#define STAGE_ARMY_MAX_SIZE 20u
#define STAGE_ARMY_LOOT_CHOICES 3u

unsigned int stage_army_size(unsigned int stage);
void stage_army_layout(unsigned int stage, uint64_t seed, unsigned int (*layout)[GRID_DIMENSION_Y]);
void stage_army_loot_choices(unsigned int stage, uint64_t seed, enum GeneralType *choices);

#endif
