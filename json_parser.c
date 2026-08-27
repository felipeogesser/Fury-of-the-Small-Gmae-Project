#include "json_parser.h"
#include "json_parser_types.h"
#include "json_parser_internal.h"
#include "field_entry.h"
#include "general_internal.h"
#include "type_tables.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

// prototypes
char *find_file_path(const char *json_file);
void check_if_keys_needs_reordering(const FieldEntry *field_table, char **keys, unsigned int keys_count);
char **object_parser(char **pp, const char *obj);
signed int key_value_parser(unsigned int i, void *memory_p, const FieldEntry *field_table, char **pp, const char **keys, const size_t keys_count);

typedef struct StringValueEntry {

    const char *string;

} StringValueEntry;

static const StringValueEntry string_value_table[] = {
    { "name" }
};
#define JSON_STRING_VALUE_TABLE_COUNT (sizeof(string_value_table) / sizeof(string_value_table[0]))

char *find_file_path(const char *json_file) {

    #define PATH_CAPACITY 1024

    char *json_file_path = calloc(1, PATH_CAPACITY);
    if (!json_file_path) {

        fprintf(stderr, "calloc failed in find_file_path\n");
        exit(EXIT_FAILURE);

    }
    char exe_path[PATH_CAPACITY];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    
    if (len == -1) {
        
        free(json_file_path);
        fprintf(stderr, "readlink failed\n");
        exit(EXIT_FAILURE);

    }
    
    exe_path[len] = '\0';

    char *dir = dirname(exe_path);

    signed int written = snprintf(json_file_path, PATH_CAPACITY, "%s/%s", dir, json_file);
    if (written < 0 || written >= PATH_CAPACITY) {

        free(json_file_path);
        fprintf(stderr, "file path too long or encoding error\n");
        exit(EXIT_FAILURE);

    }
    #undef PATH_CAPACITY

    return json_file_path;

}

char *open_read_close_file(const char *json_file) {

    char *json_file_path = find_file_path(json_file);

    FILE *file = fopen(json_file_path, "rb");
    if (!file) {
        
        printf("fopen returned null\n");
        free(json_file_path);
        return NULL;

    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (size == -1) {

        printf("ftell returned -1\n");
        fclose(file);
        free(json_file_path);
        return NULL;

    }
    fseek(file, 0, SEEK_SET);

    char *buffer = calloc(1, size + 1);
    if (!buffer) {

        printf("calloc returned null for buffer at open_read_close_file function\n");
        fclose(file);
        free(json_file_path);
        return NULL;

    }

    size_t read = fread(buffer, 1, size, file);
    if (read != (size_t)size) {
        
        printf("fread read fewer bytes than expected\n");
        free(buffer);
        fclose(file);
        free(json_file_path);
        return NULL;

    }
    buffer[size] = '\0';
    
    fclose(file);
    free(json_file_path);

    return buffer;

}

void read_file_and_retrieve_data(
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

/*const char *json_structure[] = {
        "anim",
        "sprite",
        "id",
        "rarity",
        "hp",
        "vigour",
        "attack",
        "defense",
        "evasion",
        "attack_speed",
        "general_type",
        "battalion_type"
};*/

//#define JSON_STRUCTURE_KEYS_COUNT (sizeof(json_structure) / sizeof(json_structure[0]))

void check_if_keys_needs_reordering(const FieldEntry *field_table, char **keys, unsigned int keys_count) {

    unsigned int fields_count = field_table_fields_count(field_table);

    /*_Bool run = true;
    unsigned int var = 0;
    unsigned int field_table_count = 0;
    while (run) {

        run = !(strcmp(field_table[var].key, "obj_size") == 0); 
        var++;
        field_table_count++;

    }
    field_table_count--;*/ // decrement to not count the "obj_size" field entry

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

char **object_parser(char **pp, const char *obj) {


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


signed int key_value_parser(
    unsigned int i,
    void *memory_p,
    const FieldEntry *field_table,
    char **pp,
    const char **keys,
    const size_t keys_count) {


    unsigned int fields_count = field_table_fields_count(field_table);
    /*_Bool run = true;
    unsigned int var = 0;
    unsigned int field_table_count = 0;
    while (run) {

        run = !(strcmp(field_table[var].key, "obj_size") == 0); 
        var++;
        field_table_count++;

    }
    field_table_count--;*/ // decrement to not count the "obj_size" field entry
    size_t obj_size = field_table_obj_type_size(field_table);

    char *p = *pp;
    _Bool parsing_succesful = false;

    for (unsigned int j = 0; j < keys_count; j++) {

        size_t key_str_len = strlen(keys[j]);

        while (*p != '\0') {
            printf("%c", *p);
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
                                        
                                    const GenericEntry *entry = (const GenericEntry *)((const char *)all_tables[m].entries + n * all_tables[m].entry_size);
                                    if (strcmp(string, entry->name) == 0) {
                                            
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
                        printf("parsing success\n");
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
