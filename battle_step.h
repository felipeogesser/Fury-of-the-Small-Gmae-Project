#ifndef BATTLE_STEP_H
#define BATTLE_STEP_H

#include "shared_memory_control.h"

#define BATTLE_TICK_DT (1.0f / 60.0f)
#define BATTLE_TICKS_PER_DECISION 15u
#define BATTLE_MAX_CATCH_UP_TICKS 8u

enum BattleStepResult {
    BATTLE_STEP_NEW_ORDERS = 0,
    BATTLE_STEP_CONTINUE = 1,
    BATTLE_STEP_WAITING = 2,
    BATTLE_STEP_ENDED = 3,
    BATTLE_STEP_SHUTDOWN = 4
};

void battle_step_reset(void);
void battle_step_accumulate(float frame_time, _Bool is_real_time);
_Bool battle_step_tick_due(void);
void battle_step_consume_tick(void);
enum BattleStepResult battle_step_sync(SharedMemoryControl *control, unsigned int ticks_per_decision, unsigned int timeout_ms);

#endif