#ifndef BATTLE_STEP_H
#define BATTLE_STEP_H

#include "shared_memory_control.h"

#define BATTLE_TICKS_PER_DECISION 1u

enum BattleStepResult {
    BATTLE_STEP_NEW_ORDERS = 0,
    BATTLE_STEP_CONTINUE = 1,
    BATTLE_STEP_WAITING = 2,
    BATTLE_STEP_ENDED = 3,
    BATTLE_STEP_SHUTDOWN = 4
};

void battle_step_reset(void);
enum BattleStepResult battle_step_sync(SharedMemoryControl *control, unsigned int ticks_per_decision, unsigned int max_spins);

#endif