#ifndef HEALTH_POOL_H
#define HEALTH_POOL_H

#include "field_entry_internal.h"

typedef struct HealthPool HealthPool;

typedef struct HealthPool {

    unsigned int max_hp;
    unsigned int current_hp;

} HealthPool;

extern const ComplexFieldEntry health_pool_field_table[];

#endif