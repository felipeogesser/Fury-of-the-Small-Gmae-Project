#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "field_entry_types.h"
#include <stddef.h>

void *get_json_file_data(
    const char *file_name,
    const unsigned short obj_count,
    const char *obj,
    const size_t obj_size,
    const FieldEntry *field_table,
    const size_t args_count,
    ...);

#endif