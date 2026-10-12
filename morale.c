#include "morale.h"
#include "morale_internal.h"
#include "morale_policy.h"
#include "battalion_internal.h"
#include "field_entry_internal.h"
#include <stddef.h>

#define OFFSET_OF(type, member) offsetof(type, member)
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))
#define ELEMENT_SIZE_OF(type, member) (sizeof(*(((type *)0)->member)))

const ComplexFieldEntry morale_field_table[] = {

    { "morale_state",           OFFSET_OF(Morale, state),           SIZE_OF(Morale, state),           S_INT, NULL },
    { "morale",     OFFSET_OF(Morale, current),     SIZE_OF(Morale, current),     S_INT, NULL },
    { "base",         OFFSET_OF(Morale, base),         SIZE_OF(Morale, base),         FLOAT, NULL },
    { "modifiers",        OFFSET_OF(Morale, modifiers),        SIZE_OF(Morale, modifiers),        FLOAT, NULL },
    { "obj_size", 0, sizeof(Morale), STRUCT, NULL }

};

#undef OFFSET_OF
#undef SIZE_OF
#undef ELEMENT_SIZE_OF

/*void morale_update(Battalion *battalion, signed int balance_of_power) {

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
    morale.state = evaluate_morale_state(base, current);
    battalion->morale = morale;

}*/

void morale_update(Morale *morale, const HealthPool *health_pool, signed int balance_of_power) {

    unsigned int modifiers = morale->modifiers;
    unsigned int max_hp = health_pool->max_hp;
    unsigned int current_hp = health_pool->current_hp;
    signed int modifier = 0;
    modifier += evaluate_morale_static_modifiers(modifiers);
    modifier += evaluate_morale_dynamic_modifiers(modifiers, max_hp, current_hp, balance_of_power);
    morale->current = morale->base + modifier;

    unsigned int base = morale->base;
    signed int current = morale->current;
    morale->state = evaluate_morale_state(base, current);

}
