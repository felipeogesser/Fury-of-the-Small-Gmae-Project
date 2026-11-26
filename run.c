#include "run.h"
#include "inventory.h"
#include "loot.h"
#include "rng.h"
#include "run_stats.h"
#include "save.h"
#include <stdbool.h>

typedef struct Run {

    _Bool is_active;
    unsigned int current_stage;
    _Bool has_advanced;
    _Bool is_replay;
    uint64_t seed;

} Run;

static Run run = {0};

static uint64_t new_seed(void) {

    const uint64_t high = rng_next(RNG_COSMETIC);
    const uint64_t low = rng_next(RNG_COSMETIC);
    return (high << 32) | low;

}

void run_new(void) {

    run.is_active = true;
    run.current_stage = 0;
    run.has_advanced = false;
    run.is_replay = false;
    run.seed = new_seed();
    loot_clear();
    run_stats_reset();
    inventory_reset();
    save_begin_run();

}

void run_restore(_Bool is_active, unsigned int current_stage, uint64_t seed) {

    run.is_active = is_active;
    run.is_replay = current_stage >= RUN_STAGE_COUNT;
    run.current_stage = run.is_replay ? RUN_STAGE_COUNT - 1 : current_stage;
    run.has_advanced = false;
    run.seed = seed;
    loot_clear();
    run_stats_reset();

}

_Bool run_is_active(void) {

    return run.is_active;

}

unsigned int run_current_stage(void) {

    return run.current_stage;

}

uint64_t run_seed(void) {

    return run.seed;

}

_Bool run_is_complete(void) {

    return run.current_stage >= RUN_STAGE_COUNT;

}

void run_record_result(enum EpisodeResult result) {

    if (!run.is_active || run_is_complete()) return;

    if (result == EPISODE_PLAYER_WON) {
        const unsigned int stage_won = run.current_stage;
        run.current_stage++;
        run.has_advanced = true;
        if (!run.is_replay) loot_grant(stage_won);
        save_write();
        return;
    }

    run_stats_record_loss();
    save_write();

}

_Bool run_take_advanced(void) {

    const _Bool has_advanced = run.has_advanced;
    run.has_advanced = false;
    return has_advanced;

}
