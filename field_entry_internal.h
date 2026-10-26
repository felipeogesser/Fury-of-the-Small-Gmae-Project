#ifndef FIELD_ENTRY_INTERNAL_H
#define FIELD_ENTRY_INTERNAL_H

#include "field_entry_types.h"
#include <stddef.h>

typedef struct FieldEntry {

    const char *key;
    size_t offset;
    size_t size;

} FieldEntry;

typedef struct ComplexFieldEntry {

    const char *key;
    const size_t offset;
    const size_t size;
    const enum Type type;
    const ComplexFieldEntry *struct_field_table;
    
} ComplexFieldEntry;

#endif