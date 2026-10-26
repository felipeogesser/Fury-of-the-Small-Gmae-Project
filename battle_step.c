#include "battle_step.h"

static uint64_t pending_step = 0;
static unsigned int ticks_until_decision = 0;

void battle_step_reset(void) {

    pending_step = 0;
    ticks_until_decision = 0;

}

enum BattleStepResult battle_step_sync(SharedMemoryControl *control, unsigned int ticks_per_decision, unsigned int max_spins) {

    _Bool is_running = shared_memory_control_episode_result(control) == EPISODE_RUNNING;

    if (is_running && pending_step == 0 && ticks_until_decision > 0) {
        ticks_until_decision--;
        return BATTLE_STEP_CONTINUE;
    }

    if (pending_step == 0) {
        pending_step = shared_memory_control_publish_step(control);
    }

    enum SharedMemoryWaitResult wait_result = shared_memory_control_wait_for_python(control, pending_step, max_spins);

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