#include "morale.h"
#include "morale_internal.h"
#include "morale_policy.h"
#include "battalion_internal.h"

void morale_update(Battalion *battalion, signed int balance_of_power) {

    Morale morale = battalion->morale;
    unsigned int modifiers = morale.modifiers;
    unsigned int max_hp = battalion->health_pool.max_hp;
    unsigned int current_hp = battalion->health_pool.current_hp;
    signed int modifier = 0;
    modifier += evaluate_morale_static_modifiers(modifiers);
    modifier += evaluate_morale_dynamic_modifiers(modifiers, max_hp, current_hp, balance_of_power);
    morale.current = morale.base + modifier;

    unsigned int base = morale.base;
    signed int current = morale.current;
    battalion->morale.state = evaluate_morale_state(base, current);

}
