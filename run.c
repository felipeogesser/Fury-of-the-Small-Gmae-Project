#include "run.h"
#include <stdbool.h>

typedef struct Run {

    _Bool is_active;
    unsigned int current_stage;
    _Bool has_advanced;

} Run;

static Run run = {0};

void run_new(void) {

    run.is_active = true;
    run.current_stage = 0;
    run.has_advanced = false;

}

_Bool run_is_active(void) {

    return run.is_active;

}

unsigned int run_current_stage(void) {

    return run.current_stage;

}

_Bool run_is_complete(void) {

    return run.current_stage >= RUN_STAGE_COUNT;

}

void run_record_result(enum EpisodeResult result) {

    if (!run.is_active || run_is_complete()) return;

    if (result == EPISODE_PLAYER_WON) {
        run.current_stage++;
        run.has_advanced = true;
    }

}

_Bool run_take_advanced(void) {

    const _Bool has_advanced = run.has_advanced;
    run.has_advanced = false;
    return has_advanced;

}
