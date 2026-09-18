#include "json_parser.h"
#include "json_parser_internal.h"
#include "field_entry.h"
#include "field_entry_internal.h"
#include "file_io.h"
#include "type_tables.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

// private prototypes
static void read_file_and_retrieve_data(
    void *memory_p,
    char *p,
    const FieldEntry *field_table,
    const size_t obj_count,
    const char *obj,
    char **keys,
    const size_t keys_count);
static void check_if_keys_needs_reordering(const FieldEntry *field_table, char **keys, unsigned int keys_count);
static char **object_parser(char **pp, const char *obj);
static signed int key_value_parser(unsigned int i, void *memory_p, const FieldEntry *field_table, char **pp, const char **keys, const size_t keys_count);

typedef struct StringValueEntry {

    const char *string;

} StringValueEntry;

static const StringValueEntry string_value_table[] = {
    { "name" }
};
#define JSON_STRING_VALUE_TABLE_COUNT (sizeof(string_value_table) / sizeof(string_value_table[0]))

void *get_json_file_data(
    const char *file_name,
    const unsigned short obj_count,
    const char *obj,
    const size_t obj_size,
    const FieldEntry *field_table,
    const size_t args_count,
    ...) {

    if (args_count <= 0) {

        fprintf(stderr, "at function get_json_file_data, args_count is either <= 0\n");
        return NULL;
        
    }

    char *buffer = open_read_close_file(file_name);

    if (!buffer) {
        
        fprintf(stderr, "at function get_json_file_data, inner function call open_read_close_file returned NULL for variable buffer\n");
        return NULL;

    }

    va_list args;
    va_start(args, args_count);

    char *keys[args_count];
    for (unsigned int i = 0; i < args_count; i++) {

        keys[i] = va_arg(args, char *);

    }

    char *p = buffer;
    void *memory_p = calloc(obj_count, obj_size);
    read_file_and_retrieve_data(memory_p, p, field_table, obj_count, obj, keys, args_count);

    free(buffer);
    
    va_end(args);
    
    return memory_p;

}

static void read_file_and_retrieve_data(
    void *memory_p,
    char *p,
    const FieldEntry *field_table,
    const size_t obj_count,
    const char *obj,
    char **keys,
    const size_t keys_count) {

    char **pp = &p;
    
    if (!*pp) {
        fprintf(stderr, "1: object parser returned null\n");
        exit(EXIT_FAILURE);
    }

    pp = object_parser(pp, obj);

    if (!*pp) {
        fprintf(stderr, "2: object parser returned null\n");
        exit(EXIT_FAILURE);
    }

    check_if_keys_needs_reordering(field_table, keys, keys_count);

    const char **const_keys = (const char **)keys;
    for (unsigned int i = 0; i < obj_count; i++) {

        signed int status = key_value_parser(i, memory_p, field_table, pp, const_keys, keys_count);

        if (status) {
            printf("i = %d, obj_count = %ld\n", i, obj_count);
            fprintf(stderr, "was not able to retrieve all requested data\n");
            exit(EXIT_FAILURE);
        }
    
    }

}

static void check_if_keys_needs_reordering(const FieldEntry *field_table, char **keys, unsigned int keys_count) {

    unsigned int fields_count = field_table_fields_count(field_table);

    signed int tmp[fields_count];
    memset(tmp, -1, sizeof(tmp));

    for (unsigned int j = 0; j < fields_count; j++) {

        for (unsigned int i = 0; i < keys_count; i++) {

            if (strcmp(field_table[j].key, keys[i]) == 0) {
                tmp[j] = (signed int)i;
                break;
            }

        }

    }

    char *original_keys[keys_count];
    memcpy(original_keys, keys, sizeof(char *) * keys_count);

    unsigned int out = 0;
    for (unsigned int j = 0; j < fields_count; j++) {

        if (tmp[j] != -1) {
            keys[out] = original_keys[tmp[j]];
            out++;
        }

    }

}

static char **object_parser(char **pp, const char *obj) {


    char *p = *pp;

    size_t obj_str_len = strlen(obj);
    p++; // prevents indexing to -1 when p == buffer[0];

    while (*p != '\0') {

        if (*p == *obj && *(p - 1) == '"' && *(p + obj_str_len) == '"') {

            _Bool obj_found = (obj_str_len == 1);
            char *mock_p = p;
            for (unsigned int i = 1; i < obj_str_len; i++) {

                if (*(++mock_p) != obj[i]) break;

                obj_found = (*(mock_p + 1) == '"' && obj[i + 1] == '\0');

                if (obj_found) break;

            }

            if (obj_found) {
                p = mock_p;
                break;
            } else {
                p += obj_str_len;
            }

        }

        p++;
        
    }


    if (*p == '\0') {

        p = NULL;
        fprintf(stderr, "in object parser, p reached the end of the file\n");
        exit(EXIT_FAILURE);

    }

    *pp = p;

    return pp;

}


static signed int key_value_parser(
    unsigned int i,
    void *memory_p,
    const FieldEntry *field_table,
    char **pp,
    const char **keys,
    const size_t keys_count) {


    unsigned int fields_count = field_table_fields_count(field_table);
    size_t obj_size = field_table_obj_type_size(field_table);

    char *p = *pp;
    _Bool parsing_succesful = false;

    for (unsigned int j = 0; j < keys_count; j++) {

        size_t key_str_len = strlen(keys[j]);

        while (*p != '\0') {

            if (*p == *(keys[j]) && *(p - 1) == '"' && *(p + key_str_len) == '"') {

                _Bool key_found = (key_str_len == 1);
                char *mock_p = p;
                for (unsigned int k = 1; k < key_str_len; k++) {

                    if (*(++mock_p) != keys[j][k]) break;

                    key_found = (*(mock_p + 1) == '"' && keys[j][k + 1] == '\0');

                    if (key_found) break;

                }

                if (key_found) {

                    p = mock_p + 2;
                    
                    while (*p == ':' || *p == ' ') {

                        p++;

                    }

                    unsigned int l = 0;
                    for (; l < fields_count; l++) {

                        if (strcmp(field_table[l].key, keys[j]) == 0) break;

                    }

                    if (l >= fields_count) {

                        fprintf(stderr, "either key arg doesnt exist or key missing in fiedld table\n");
                        exit(EXIT_FAILURE);

                    }

                    if (*p == '"') {

                        p++;

                        _Bool value_should_continue_be_a_string = false;
                        for (unsigned int m = 0; m < JSON_STRING_VALUE_TABLE_COUNT && !value_should_continue_be_a_string; m++) {

                            value_should_continue_be_a_string |= (strcmp(keys[j], string_value_table[m].string) == 0);

                        }

                        char string_array[32];
                        unsigned char idx = 0;
                        while(*p != '"') {

                            string_array[idx] = *p;
                            idx++;
                            p++;

                        }

                        string_array[idx] = '\0';
                        char *string = string_array;

                        if (value_should_continue_be_a_string) {

                            char *dest = (char *)memory_p + obj_size * i + field_table[l].offset;
                            memset(dest, 0, field_table[l].size);
                            strcpy(dest, string);

                        } else {

                            for (unsigned int m = 0; m < all_tables_count; m++) {
                                
                                for (unsigned int n = 0; n < all_tables[m].entry_count; n++) {
                                        // future refactor: add another check to compare string with all_table entry_name to double check key-value
                                    const GenericEntry *entry = (const GenericEntry *)((const char *)all_tables[m].entries + n * all_tables[m].entry_size);
                                    if (strcmp(string, entry->name) == 0) {
                                        // future fix: this doesnt check for if string refers to a struct field,
                                        // so this part just writes in the struct without knowing its inner fields
                                        // luckily, the structs have their first fields being the ones expected by this code    
                                        signed int *dest = (signed int *)((char *)memory_p + obj_size * i + field_table[l].offset);
                                        memset(dest, 0, field_table[l].size);
                                        *dest = entry->value;
                                        break;

                                    }

                                }

                            }

                        }

                    } else {
                        
                        _Bool float_number = false;
                        char string_array[32];
                        unsigned char idx = 0;
                        while (*p != ',' && *p != ' ' && *p != '}') {

                            float_number |= (*p == '.');
                            string_array[idx] = *p;
                            idx++;
                            p++;

                        }

                        string_array[idx] = '\0';
                        char *string = string_array;

                        char *endptr = NULL;
                        if (!float_number) {

                            long value = strtol(string, &endptr, 10);
                            char *dest = (char *)memory_p + obj_size * i + field_table[l].offset;
                            size_t field_size = field_table[l].size;

                            if (sizeof(value) >= field_size) {

                                memset(dest, 0, field_size);
                                memcpy(dest, &value, field_size);

                            } else {

                                memset(dest, 0, field_size);
                                memcpy(dest, &value, sizeof(value));

                            }

                        } else {

                            char *dest = (char *)memory_p + obj_size * i + field_table[l].offset;
                            size_t field_size = field_table[l].size;
                            if (field_size == sizeof(float)) {

                                float value = (float)strtod(string, &endptr);
                                memset(dest, 0, field_size);
                                memcpy(dest, &value, field_size);

                            } else if (field_size == sizeof(double)) {

                                double value = strtod(string, &endptr);
                                memset(dest, 0, field_size);
                                memcpy(dest, &value, field_size);

                            } else {

                                fprintf(stderr, "field size is neither float or double size.\n");
                                exit(EXIT_FAILURE);

                            }

                        }

                    }

                    if (j + 1 == keys_count) {
                        
                        *pp = p;
                        parsing_succesful = true;
                        return 0;
                        
                    }

                    break;

                }

            }


            p++;
        
        }

    }

    if (!parsing_succesful) {
    
        *pp = p;

        return 1;

    }

    *pp = p;

    return 0;

}
