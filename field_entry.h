#ifndef FIELD_ENTRY_H
#define FIELD_ENTRY_H

#include <stddef.h>

typedef struct FieldEntry {
    const char *key;
    size_t offset;
    size_t size;
} FieldEntry;

unsigned int field_table_fields_count(const FieldEntry *field_table);
size_t field_table_obj_type_size(const FieldEntry *field_table);
#endif