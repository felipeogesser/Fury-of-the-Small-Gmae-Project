#include "loot.h"
#include "engine_internal.h"
#include "inventory.h"
#include "run.h"
#include "stage_army.h"
#include <stdbool.h>

typedef struct LootReveal {

    enum GeneralType type;
    _Bool is_pending;

} LootReveal;

static LootReveal reveal = {GENERAL_TYPE_UNKNOWN, false};

void loot_grant(unsigned int stage_won) {

    reveal.is_pending = false;
    if (engine.has_program_been_initialized_with_ai_bypass) return;

    const enum GeneralType type = stage_army_loot(stage_won, run_seed());
    if (!inventory_add(type)) return;

    reveal.type = type;
    reveal.is_pending = true;

}

_Bool loot_peek_reveal(enum GeneralType *type) {

    if (!reveal.is_pending) return false;
    *type = reveal.type;
    return true;

}

void loot_dismiss_reveal(void) {

    reveal.is_pending = false;

}
