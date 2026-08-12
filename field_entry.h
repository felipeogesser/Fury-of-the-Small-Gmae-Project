#ifndef FIELD_ENTRY_H
#define FIELD_ENTRY_H

#include <stddef.h>

typedef struct FieldEntry {
    const char *key;
    size_t offset;
    size_t size;
} FieldEntry;

#endif