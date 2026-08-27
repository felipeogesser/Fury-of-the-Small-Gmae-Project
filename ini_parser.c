#include "ini_parser.h"
#include "field_entry.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
// private prototypes
static char *find_file_path(const char *json_file);

char *open_read_close_ini_file(const char *ini_file) {

    char *ini_file_path = find_file_path(ini_file);

    FILE *file = fopen(ini_file_path, "rb");
    if (!file) {
        
        printf("fopen returned null\n");
        free(ini_file_path);
        return NULL;

    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (size == -1) {

        printf("ftell returned -1\n");
        fclose(file);
        free(ini_file_path);
        return NULL;

    }
    fseek(file, 0, SEEK_SET);

    char *buffer = calloc(1, size + 1);
    if (!buffer) {

        printf("calloc returned null for buffer at open_read_close_ini_file function\n");
        fclose(file);
        free(ini_file_path);
        return NULL;

    }

    size_t read = fread(buffer, 1, size, file);
    if (read != (size_t)size) {
        
        printf("fread read fewer bytes than expected\n");
        free(buffer);
        fclose(file);
        free(ini_file_path);
        return NULL;

    }
    buffer[size] = '\0';
    
    fclose(file);
    free(ini_file_path);

    return buffer;

}

static char *find_file_path(const char *json_file) {

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

enum Scanning {

    KEY = 0,
    VALUE

};


// very simple parser. for values, it only knows how to handle numbers.
// it doesnt knows spaces, characters or anything else that is neigher a number or '.' from a float
// but it knows '=' and '\n' but isolated in certain circunstances
// handles almost no errors like typing or logic errors
// numbers cant be longer than 7 digits. for floats, cant be 6 or more (because of '.')
void update_battlefield_formation_with_ini_values(const char *buffer, void *obj, const FieldEntry *field_table) {

    #define KEY_LENGH 32
    #define VALUE_LENGH 8
    char key[KEY_LENGH] = {'\0'};
    char value[VALUE_LENGH] = {'\0'};
    const char *p = buffer;
    enum Scanning scanning = KEY;
    _Bool is_float = false;
    unsigned int idx = 0;
    unsigned int fields_count = field_table_fields_count(field_table);
    //size_t obj_size = field_table_obj_type_size(field_table);
    unsigned int field_table_idx = 0;
    while (*p != '\0') {

        if (scanning == KEY) {

            if (*p == '=') {
                
                for (unsigned int i = 0; i < fields_count; i++) {
                
                    if (strcmp(key, field_table[i].key) == 0) {

                        field_table_idx = i;
                        break;

                    }
                    if (i == fields_count - 1) {
                        
                        printf("field table fields count = %d, i = %d\n", fields_count, i);
                        printf("key: %s, field table key: %s\n", key, field_table[i].key);
                        fprintf(stderr, "at file ini_parser function update_battlefield_formation_with_ini_values, ini file contains key that doesnt exist in the field table\n");
                        exit(EXIT_FAILURE);

                    }
                }
                scanning = VALUE;
                p++;
                idx = 0;
                continue;

            }
            
            key[idx++] = *p;
            p++;

        } else {

            if (*p == '\n') {

                if (is_float) {

                    float *dest = (float *)((char *)obj + field_table[field_table_idx].offset);
                    memset(dest, 0, field_table[field_table_idx].size);
                    *dest = atof(value);

                } else {

                    signed int *dest = (signed int *)((char *)obj + field_table[field_table_idx].offset);
                    memset(dest, 0, field_table[field_table_idx].size);
                    *dest = atoi(value);

                }

                is_float = false;
                memset(key, 0, KEY_LENGH);
                memset(value, 0, VALUE_LENGH);
                scanning = KEY;
                idx = 0;
                p++;
                continue;

            }

            value[idx++] = *p;
            is_float |= (*p == '.');
            p++;

        }
        
    }

    #undef KEY_LENGH
    #undef VALUE_LENGH

}