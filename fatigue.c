#include "fatigue.h"
#include "fatigue_internal.h"
#include "fatigue_policy.h"
#include "battalion_internal.h"

void fatigue_update(Battalion *battalion, float dt) {

    unsigned int base = battalion->fatigue.base;
    float current = battalion->fatigue.current;
    _Bool is_active = battalion->is_active;
    battalion->fatigue.current = evaluate_fatigue_current(base, current, is_active, dt);
    
    current = battalion->fatigue.current;
    battalion->fatigue.state = evaluate_fatigue_state(base, current);

}