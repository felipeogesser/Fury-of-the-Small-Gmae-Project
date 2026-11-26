#include "loot.h"
#include "engine_internal.h"
#include "inventory.h"
#include "run.h"
#include "run_stats.h"
#include "save.h"
#include <stdbool.h>

typedef struct LootOffer {

    _Bool is_pending;
    unsigned int stage;
    enum GeneralType choices[STAGE_ARMY_LOOT_CHOICES];

} LootOffer;

static LootOffer offer = {0};

void loot_clear(void) {

    offer.is_pending = false;

}

void loot_grant(unsigned int stage_won) {

    offer.is_pending = false;
    if (engine.has_program_been_initialized_with_ai_bypass) return;
    if (stage_won >= RUN_STAGE_COUNT || inventory_count() >= INVENTORY_CAPACITY) return;

    stage_army_loot_choices(stage_won, run_seed(), offer.choices);
    offer.stage = stage_won;
    offer.is_pending = true;

}

_Bool loot_has_pending(void) {

    return offer.is_pending;

}

unsigned int loot_pending_stage(void) {

    return offer.stage;

}

enum GeneralType loot_choice(unsigned int index) {

    if (!offer.is_pending || index >= STAGE_ARMY_LOOT_CHOICES) return GENERAL_PADDING_FOR_ZERO;
    return offer.choices[index];

}

void loot_take(unsigned int index) {

    if (!offer.is_pending || index >= STAGE_ARMY_LOOT_CHOICES) return;

    const enum GeneralType type = offer.choices[index];
    offer.is_pending = false;
    if (!inventory_add(type)) return;
    run_stats_record_pick(offer.stage, type);
    save_write();

}
