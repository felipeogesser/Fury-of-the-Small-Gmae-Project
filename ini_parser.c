#include "ini_parser.h"
#include "field_entry.h"
#include "field_entry_internal.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

// very simple parser. for values, it only knows how to handle numbers.
// it doesnt knows spaces, characters or anything else that is neigher a number or '.' from a float
// handles almost no errors like typing or logic errors
// numbers cant be longer than 7 digits. for floats, cant be 6 or more (because of '.')
void update_battlefield_formation_with_ini_values(const char *buffer, const char *obj_str_lit, void *obj, const FieldEntry *field_table) {

    if (buffer[0] != '[') {

        fprintf(stderr, "ini_parser: file must begin with an object header '['\n");
        exit(EXIT_FAILURE);

    }

    size_t str_len = strlen(obj_str_lit);

    if (str_len == 0) {

        fprintf(stderr, "ini_parser: string passed has lengh zero\n");
        exit(EXIT_FAILURE);

    }

    #define STR_MAX_LEN 31
    if (str_len > STR_MAX_LEN) {
    #undef STR_MAX_LEN
    
        fprintf(stderr, "ini_parser: string passed exceeds lengh of 31 characters\n");
        exit(EXIT_FAILURE);

    }

    enum Scanning {

        KEY,
        VALUE,
        OBJECT

    };

    #define KEY_LENGH 32
    #define VALUE_LENGH 8
    char key[KEY_LENGH] = {'\0'};
    char value[VALUE_LENGH] = {'\0'};
    const char *p = buffer;
    //p++; // prevents indexing out of bounds when doing "*(p - 1)"
    enum Scanning scanning = OBJECT;
    _Bool is_float = false;
    unsigned int idx = 0;
    unsigned int fields_count = field_table_fields_count(field_table);
    //size_t obj_size = field_table_obj_type_size(field_table);
    unsigned int field_table_idx = 0;

    while (*p != '\0') {

        if (scanning == OBJECT) {

            if (*p == '[') {
                
                p++;
                
                if (*(p + str_len) == ']') {

                    if (memcmp(p, obj_str_lit, str_len) == 0) {

                        scanning = KEY;

                    }
                    
                    p += str_len + 2; // 2 is a magic number that skips ']' and '\n' after the .ini section name
                    continue;

                }

            }

            p++;

        } else if (scanning == KEY) {

            if (*p == '=') {
                key[idx] = '\0';
                for (unsigned int i = 0; i < fields_count; i++) {

                    if (strcmp(key, field_table[i].key) == 0) {

                        field_table_idx = i;
                        break;

                    }
                    if (i == fields_count - 1) {
                        
                        printf("field table fields count = %u, i = %u\n", fields_count, i);
                        printf("key: %s, field table key: %s\n", key, field_table[i].key);
                        fprintf(stderr, "at file ini_parser function update_battlefield_formation_with_ini_values, ini file contains key that doesnt exist in the field table\n");
                        exit(EXIT_FAILURE);

                    }
                }
                memset(key, 0, KEY_LENGH);
                scanning = VALUE;
                p++;
                idx = 0;
                continue;

            }
            key[idx++] = *p;
            p++;

        } else if (scanning == VALUE) {

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
