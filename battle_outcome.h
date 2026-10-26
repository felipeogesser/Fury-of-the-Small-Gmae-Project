#ifndef BATTLE_OUTCOME_H
#define BATTLE_OUTCOME_H

#include <stdint.h>

#include "shared_memory_control.h"
#include "table_types.h"

#define BATTLE_MAX_TICKS 36000u

enum EpisodeResult battle_outcome_evaluate(const Table *player_tables, const Table *enemy_tables, unsigned int battalion_count, uint64_t tick);

#endif