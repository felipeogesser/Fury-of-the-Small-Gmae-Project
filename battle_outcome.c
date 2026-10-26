#include "battle_outcome.h"
#include "battalion_table_internal.h"
#include "table_internal.h"

enum EpisodeResult battle_outcome_evaluate(const Table *player_tables, const Table *enemy_tables, unsigned int battalion_count, uint64_t tick) {

    unsigned int player_present = 0;
    unsigned int enemy_present = 0;

    for (unsigned int i = 0; i < battalion_count; i++) {

        player_present += player_tables->battalion_tables.friendly_dynamic.is_present[i];
        enemy_present += enemy_tables->battalion_tables.friendly_dynamic.is_present[i];

    }

    if (player_present == 0 && enemy_present == 0) return EPISODE_DRAW;
    if (player_present == 0) return EPISODE_ENEMY_WON;
    if (enemy_present == 0) return EPISODE_PLAYER_WON;
    if (tick >= BATTLE_MAX_TICKS) return EPISODE_TIMEOUT;

    return EPISODE_RUNNING;

}