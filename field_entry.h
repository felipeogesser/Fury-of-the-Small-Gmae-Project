#ifndef FIELD_ENTRY_H
#define FIELD_ENTRY_H

#include "field_entry_types.h"
#include <stddef.h>

unsigned int field_table_fields_count(const FieldEntry *field_table);
size_t field_table_obj_type_size(const FieldEntry *field_table);
#endif