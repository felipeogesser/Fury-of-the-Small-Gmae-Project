#include "fatigue.h"
#include "fatigue_internal.h"
#include "fatigue_policy.h"
#include "battalion_internal.h"
#include "field_entry_internal.h"
#include <stddef.h>

#define OFFSET_OF(type, member) offsetof(type, member)
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))
#define ELEMENT_SIZE_OF(type, member) (sizeof(*(((type *)0)->member)))

const ComplexFieldEntry fatigue_field_table[] = {

    { "fatigue_state",           OFFSET_OF(Fatigue, max_hp),           SIZE_OF(Fatigue, state),           S_INT, NULL },
    { "fatigue_base",     OFFSET_OF(Fatigue, base),     SIZE_OF(Fatigue, base),     U_INT, NULL },
    { "fatigue",     OFFSET_OF(Fatigue, current),     SIZE_OF(Fatigue, current),     FLOAT, NULL },
    { "obj_size", 0, sizeof(Fatigue), STRUCT, NULL }

};

#undef OFFSET_OF
#undef SIZE_OF
#undef ELEMENT_SIZE_OF

/*void fatigue_update(Battalion *battalion, float dt) {

    unsigned int base = battalion->fatigue.base;
    float current = battalion->fatigue.current;
    _Bool is_active = battalion->is_active;
    battalion->fatigue.current = evaluate_fatigue_current(base, current, is_active, dt);
    
    current = battalion->fatigue.current;
    battalion->fatigue.state = evaluate_fatigue_state(base, current);

}*/

void fatigue_update(Fatigue *fatigue, _Bool is_active, _Bool is_fighting, float dt) {

    unsigned int base = fatigue->base;
    float current = fatigue->current;
    fatigue->current = evaluate_fatigue_current(base, current, is_active, is_fighting, dt);
    
    current = fatigue->current;
    fatigue->state = evaluate_fatigue_state(base, current);

}