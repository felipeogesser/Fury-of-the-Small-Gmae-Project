#ifndef FORMATION_H
#define FORMATION_H

#include "table_types.h"

void formation_update(Table *tables,
                const unsigned int battalion_count, const unsigned int unit_count,
                const unsigned int battalion_size,
                const float unit_w, const float unit_h, const float padding_x, const float padding_y,
                const float distance, const float max_speed);

#endif