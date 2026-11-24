#ifndef INVENTORY_H
#define INVENTORY_H

#include "starting_army.h"

void *inventory_init(void);

void inventory_set_starting_army(enum StartingArmy army);

#endif