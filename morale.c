#include "morale.h"
#include "morale_policy.h"
#include "battalion_internal.h"
#include "battalion_types.h"

void morale_update(Battalion *battalion, signed int balance_of_power) {

    unsigned int modifiers = battalion->morale.modifiers;
    unsigned int max_hp = battalion->health_pool.max_hp;
    unsigned int current_hp = battalion->health_pool.current_hp;
    signed int modifier = 0;
    modifier += evaluate_static_morale_modifiers(modifiers);
    modifier += evaluate_dynamic_morale_modifiers(modifiers, max_hp, current_hp, balance_of_power);
    morale->current = morale->base + modifier;

}
