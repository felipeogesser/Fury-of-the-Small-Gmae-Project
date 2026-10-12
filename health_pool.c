#include "health_pool.h"
#include "field_entry_internal.h"
#include <stddef.h>

#define OFFSET_OF(type, member) offsetof(type, member)
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))
#define ELEMENT_SIZE_OF(type, member) (sizeof(*(((type *)0)->member)))

const ComplexFieldEntry health_pool_field_table[] = {

    { "max_hp",           OFFSET_OF(HealthPool, max_hp),           SIZE_OF(HealthPool, max_hp),           U_INT, NULL },
    { "hp",     OFFSET_OF(HealthPool, current_hp),     SIZE_OF(HealthPool, current_hp),     U_INT, NULL },
    { "obj_size", 0, sizeof(HealthPool), STRUCT, NULL }

};

#undef OFFSET_OF
#undef SIZE_OF
#undef ELEMENT_SIZE_OF