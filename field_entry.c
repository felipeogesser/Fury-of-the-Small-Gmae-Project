#include "field_entry.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_ITER 100

unsigned int field_table_fields_count(const FieldEntry *field_table) {

    _Bool found_sentinel_field = false;
    unsigned int iterations = 0;
    unsigned int idx = 0;
    signed int field_table_count = 0;
    while (!found_sentinel_field && iterations < MAX_ITER) {

        found_sentinel_field = (strcmp(field_table[idx].key, "obj_size") == 0); 
        idx++;
        field_table_count++;
        iterations++;

    }

    if (iterations >= MAX_ITER) {

        printf("iterations = %d\n", iterations);
        fprintf(stderr, "At file field_entry.c function field_table_fields_count, while loop tried to iterate over max iter limit\n");
        exit(EXIT_FAILURE);

    }

    if (field_table_count <= 1) {

        printf("field_table_count = %d\n", field_table_count);
        fprintf(stderr, "At file field_entry.c function field_table_fields_count, the field table passed as args had zero fields\n");
        exit(EXIT_FAILURE);

    }

    return --field_table_count;

}

size_t field_table_obj_type_size(const FieldEntry *field_table) {


    _Bool found_sentinel_field = false;
    unsigned int iterations = 0;
    unsigned int idx = 0;
    signed int obj_size = 0;
    while (!found_sentinel_field && iterations < MAX_ITER) {

        if (strcmp(field_table[idx].key, "obj_size") == 0) {
            
            found_sentinel_field = true;
            obj_size = field_table[idx].size;

        }

        idx++;
        iterations++;

    }

    if (iterations >= MAX_ITER) {

        printf("iterations = %d\n", iterations);
        fprintf(stderr, "At file field_entry.c function field_table_obj_type_size, while loop tried to iterate over max iter limit\n");
        exit(EXIT_FAILURE);

    }

    if (obj_size <= 0) {

        printf("object size = %d\n", obj_size);
        fprintf(stderr, "At file field_entry.c function field_table_obj_type_size, the field table reports its object having a size of zero bytes or negative size\n");
        exit(EXIT_FAILURE);

    }

    return obj_size;

}

#undef MAX_ITER

