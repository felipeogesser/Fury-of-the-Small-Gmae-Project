#include "battalion_policy.h"
#include "battalion_internal.h"
#include "morale_internal.h"
#include "table_internal.h"
#include <stdbool.h>

// private prototypes
void update_rally(Table *tables, float dt);
//static void update_lifecycle(Battalion *battalion);
static Rally evaluate_rally(Rally rally, enum BattalionLifecycle lifecycle, signed int morale_current, float dt);
//static enum BattalionLifecycle evaluate_lifecycle(BattalionStatistics statistics, _Bool rallying, signed int morale_current);

/*void battalion_update_state(Battalion *battalion, float dt) {

    //battalion->state.movement = evaluate_movement(battalion->state.movement);
    //battalion->state.headless = evaluate_headless(battalion->state.headless);
    
    update_rally(battalion, dt);
    update_lifecycle(battalion);

}*/

//void battalion_update_flags(Battalion *battalion, float dt) {}

void update_rally(Table *tables, float dt) {

    Rally rally = {0};
    rally.rally_timer = tables->battalion_tables.friendly_dynamic.rally_timer;
    rally.is_rallying = tables->battalion_tables.friendly_dynamic.is_rallying;
    enum BattalionLifecycle lifecycle = tables->battalion_tables.friendly_dynamic.lifecycle_state;
    signed int morale_current = (signed int)tables->battalion_tables.friendly_dynamic.morale_current;

    rally = evaluate_rally(rally, lifecycle, morale_current, dt);

    tables->battalion_tables.friendly_dynamic.rally_timer = rally.rally_timer;
    tables->battalion_tables.friendly_dynamic.is_rallying = rally.is_rallying;

}

/*static void update_lifecycle(Battalion *battalion) {

    BattalionStatistics statistics = battalion->statistics;
    _Bool rallying = battalion->state.rally.rallying;
    signed int morale_current = battalion->morale.current;
    
    battalion->state.lifecycle = evaluate_lifecycle(statistics, rallying, morale_current);

}*/

static Rally evaluate_rally(Rally rally, enum BattalionLifecycle lifecycle, signed int morale_current, float dt) {

    if (!rally.rallying) {

        if (morale_current > 0 && lifecycle == ROUTING) {

            rally.rallying = true;
            rally.timer = RALLY_TIMER;

        }

    } else {

        if (morale_current > 0 && lifecycle == ROUTING) {

            rally.timer -= dt;
            if (rally.timer <= 0.0f) {
                
                rally.timer = 0.0f;
                rally.rallying = false;
            
            }

        } else {

            rally.timer = RALLY_TIMER;
            rally.rallying = false;

        }

    }

    return rally;

}

/*static enum BattalionLifecycle evaluate_lifecycle(BattalionStatistics statistics, _Bool rallying, signed int morale_current) {

    unsigned int unit_base_count = statistics.unit_base_count;
    unsigned int unit_dead_count = statistics.unit_dead_count;
    unsigned int unit_withdrawed_count = statistics.unit_withdrawed_count;

    if (unit_withdrawed_count == 0 && unit_dead_count == unit_base_count) return WIPED_OUT;
    if (unit_withdrawed_count + unit_dead_count == unit_base_count) return WITHDRAWN;
    if (morale_current < 0 || (rallying && morale_current > 0)) return ROUTING;
    return NORMAL;

}*/