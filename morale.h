#ifndef MORALE_H
#define MORALE_H

//#include "battalion_types.h"
#include "morale_types.h"
#include "health_pool.h"

//void morale_update(Battalion *battalion, signed int balance_of_power);
void morale_update(Morale *morale, const HealthPool *health_pool, signed int balance_of_power);

#endif