#ifndef AOS_SOA_TRANSFORMER
#define AOS_SOA_TRANSFORMER

#include "field_entry_types.h"
#include <stddef.h>

void aos_soa_transform(
    void *aos,
    void **soa,
    const ComplexFieldEntry *aos_field_table,
    const ComplexFieldEntry *soa_field_table,
    size_t obj_count,
    unsigned int args_count, ...);

#endif