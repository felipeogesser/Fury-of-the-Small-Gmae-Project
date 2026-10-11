#ifndef FORMATION_H
#define FORMATION_H

#include "table_types.h"

void formation_update(Table *tables,
                unsigned int battalion_count, unsigned int unit_count,
                unsigned int battalion_size,
                float unit_w, float unit_h, float padding_x, float padding_y,
                float distance, float max_speed);

#endif