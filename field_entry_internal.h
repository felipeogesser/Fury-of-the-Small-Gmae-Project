#ifndef FIELD_ENTRY_INTERNAL_H
#define FIELD_ENTRY_INTERNAL_H

#include "field_entry_types.h"
#include <stddef.h>

typedef struct FieldEntry {

    const char *key;
    size_t offset;
    size_t size;

} FieldEntry;

#endif