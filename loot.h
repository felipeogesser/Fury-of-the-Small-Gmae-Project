#ifndef LOOT_H
#define LOOT_H

#include "general_types.h"
#include "stage_army.h"

void loot_grant(unsigned int stage_won);
void loot_clear(void);

_Bool loot_has_pending(void);
unsigned int loot_pending_stage(void);
enum GeneralType loot_choice(unsigned int index);
void loot_take(unsigned int index);

#endif
