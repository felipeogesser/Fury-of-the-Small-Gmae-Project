#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "json_parser_types.h"
#include "field_entry.h"
#include "general_types.h"
#include <stddef.h>

void read_file_and_retrieve_data(void *memory_p, char *p, const FieldEntry *field_table, const size_t obj_count, const char *obj, char **keys, const size_t keys_count);
char *open_read_close_file(const char *json_file);

#endif