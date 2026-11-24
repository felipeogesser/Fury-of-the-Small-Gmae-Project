#ifndef BATTLEFIELD_PICK_H
#define BATTLEFIELD_PICK_H

#include "table_internal.h"

_Bool battlefield_pick_battalion(const Table *player_tables, const Table *enemy_tables, unsigned int battalion_count, unsigned int battalion_size, int screen_x, int screen_y, _Bool *is_enemy, unsigned int *index);

#endif
