#ifndef FLANK_H
#define FLANK_H

#include "flank_types.h"
#include "table_types.h"

void flank_update(Table *player_tables, Table *enemy_tables, unsigned int battalion_count, unsigned int battalion_size);
float flank_hit_bonus(enum FlankState flank);
float flank_damage_multiplier(enum FlankState flank, _Bool is_cavalry);

#endif
