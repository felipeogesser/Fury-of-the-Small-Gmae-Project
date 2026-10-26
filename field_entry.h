#ifndef FIELD_ENTRY_H
#define FIELD_ENTRY_H

#include "field_entry_types.h"
#include <stddef.h>

unsigned int field_table_fields_count(const FieldEntry *field_table);
size_t field_table_obj_type_size(const FieldEntry *field_table);
//void copy_aos_key_fields_to_soa(const ComplexFieldEntry *field_table, void **soa, void *aos, unsigned int obj_count, char **keys, unsigned int keys_count);
void copy_aos_key_fields_to_soa(const ComplexFieldEntry *soa_field_table, const ComplexFieldEntry *aos_field_table, void **soa, void *aos, unsigned int obj_count, char **keys, unsigned int keys_count);
unsigned int field_table_check_if_keys_exist(const ComplexFieldEntry *field_table, const char **keys, const unsigned int keys_count);
void field_table_match_keys_order(const ComplexFieldEntry *field_table, char **keys, unsigned int keys_count);
//void convert_type(void *dest, const void *src, enum Type dest_type, enum Type src_type);
void subtract_type(void *dest, const void *src1, const void *src2,
                          enum Type dest_type, enum Type src1_type, enum Type src2_type);
size_t complex_field_table_obj_type_size(const ComplexFieldEntry *field_table);
unsigned int complex_field_table_fields_count(const ComplexFieldEntry *field_table);

#endif