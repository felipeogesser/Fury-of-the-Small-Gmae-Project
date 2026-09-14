#ifndef INI_PARSER_H
#define INI_PARSER_H

#include "field_entry.h"

char *open_read_close_ini_file(const char *ini_file);
void update_battlefield_formation_with_ini_values(const char *buffer, const char *obj_str_lit, void *obj, const FieldEntry *field_table);

#endif