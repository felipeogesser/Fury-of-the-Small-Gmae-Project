#ifndef LOOT_H
#define LOOT_H

#include "general_types.h"

void loot_grant(unsigned int stage_won);
_Bool loot_peek_reveal(enum GeneralType *type);
void loot_dismiss_reveal(void);

#endif
