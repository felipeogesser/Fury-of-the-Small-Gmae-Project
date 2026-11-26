#ifndef INVENTORY_H
#define INVENTORY_H

#include "general_types.h"
#include "starting_army.h"

#define INVENTORY_CAPACITY 64u

void *inventory_init(void);

void inventory_set_starting_army(enum StartingArmy army);
enum StartingArmy inventory_starting_army(void);

void inventory_reset(void);
_Bool inventory_add(enum GeneralType type);
void inventory_restore(const enum GeneralType *types, unsigned int count);
unsigned int inventory_count(void);
enum GeneralType inventory_type_at(unsigned int index);

#endif
