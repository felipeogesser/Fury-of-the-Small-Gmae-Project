#include "battle_step.h"

static uint64_t pending_step = 0;
static unsigned int ticks_until_decision = 0;
static float accumulated_time = 0.0f;

void battle_step_reset(void) {

    pending_step = 0;
    ticks_until_decision = 0;
    accumulated_time = 0.0f;

}

void battle_step_accumulate(float frame_time, _Bool is_real_time) {

    if (!is_real_time) {
        accumulated_time = BATTLE_TICK_DT;
        return;
    }

    accumulated_time += frame_time;
    if (accumulated_time > BATTLE_TICK_DT * BATTLE_MAX_CATCH_UP_TICKS) accumulated_time = BATTLE_TICK_DT * BATTLE_MAX_CATCH_UP_TICKS;

}

_Bool battle_step_tick_due(void) {

    return accumulated_time >= BATTLE_TICK_DT;

}

void battle_step_consume_tick(void) {

    accumulated_time -= BATTLE_TICK_DT;

}

enum BattleStepResult battle_step_sync(SharedMemoryControl *control, unsigned int ticks_per_decision, unsigned int timeout_ms) {

    _Bool is_running = shared_memory_control_episode_result(control) == EPISODE_RUNNING;

    if (is_running && pending_step == 0 && ticks_until_decision > 0) {
        ticks_until_decision--;
        return BATTLE_STEP_CONTINUE;
    }

    if (pending_step == 0) {
        pending_step = shared_memory_control_publish_step(control);
    }

    enum SharedMemoryWaitResult wait_result = shared_memory_control_wait_for_python(control, pending_step, timeout_ms);

    if (wait_result == SHARED_MEMORY_WAIT_SHUTDOWN) return BATTLE_STEP_SHUTDOWN;
    if (wait_result == SHARED_MEMORY_WAIT_TIMEOUT) return BATTLE_STEP_WAITING;

    pending_step = 0;

    if (!is_running) {
        shared_memory_control_end_battle(control);
        ticks_until_decision = 0;
        return BATTLE_STEP_ENDED;
    }

    ticks_until_decision = (ticks_per_decision > 0) ? ticks_per_decision - 1 : 0;

    return BATTLE_STEP_NEW_ORDERS;

}