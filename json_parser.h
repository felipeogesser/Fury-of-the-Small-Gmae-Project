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
//void read_file_and_retrieve_data(void *memory_p, char *p, const FieldEntry *field_table, const size_t obj_count, const char *obj, char **keys, const size_t keys_count);
//char *open_read_close_file(const char *json_file);

#endif