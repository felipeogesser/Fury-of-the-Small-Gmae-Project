#ifndef STEERING_H
#define STEERING_H

#include "table_types.h"

void steer_headless_units(Table *tables, const unsigned int battalion_count,
                        const unsigned int unit_count, const unsigned int battalion_size,
                        const float max_speed);

#endif