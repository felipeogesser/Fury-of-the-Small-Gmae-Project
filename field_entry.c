#include "field_entry.h"
#include "field_entry_internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// private prototypes
static _Bool recursive_field_table_key_search(const ComplexFieldEntry *field_table, const char *key);
static void convert_type(void *dest, const void *src, enum Type dest_type, enum Type src_type);
static void store_signed(void *dest, enum Type t, long long v);
static void store_unsigned(void *dest, enum Type t, unsigned long long v);
static void store_double(void *dest, enum Type t, double v);

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

void copy_aos_key_fields_to_soa(const ComplexFieldEntry *soa_field_table, const ComplexFieldEntry *aos_field_table, void **soa, void *aos, unsigned int obj_count, char **keys, unsigned int keys_count) {

    struct Vec3OffsetSizeType {

        size_t offset, size;
        enum Type type;

    };
    struct Vec3OffsetSizeType aos_field[keys_count];
    struct Vec3OffsetSizeType soa_field[keys_count];

    memset(aos_field, 0, keys_count * sizeof(struct Vec3OffsetSizeType));
    memset(soa_field, 0, keys_count * sizeof(struct Vec3OffsetSizeType));

    unsigned int field_count = 0;
    
    field_count = complex_field_table_fields_count(aos_field_table);
    for (unsigned int i = 0; i < keys_count; i++) {

        for (unsigned int j = 0; j < field_count; j++) {
        
            if (aos_field_table[j].type == STRUCT) {
                
                ComplexFieldEntry *struct_field_table = ((ComplexFieldEntry *)(aos_field_table[j].struct_field_table));

                if (struct_field_table == NULL) {
                    fprintf(stderr, "struct_field_table pointer is NULL\n");
                    printf("key of AoS NULL struct: %s\n", aos_field_table[j].key);
                }

                unsigned int struct_field_count = complex_field_table_fields_count(struct_field_table);

                for (unsigned int k = 0; k < struct_field_count; k++) {

                    if (strcmp(keys[i], struct_field_table[k].key) == 0) {

                        size_t offset = aos_field_table[j].offset + struct_field_table[k].offset;
                        aos_field[i].offset = offset;
                        aos_field[i].size = struct_field_table[k].size;
                        aos_field[i].type = struct_field_table[k].type;
                        goto HERE;

                    }

                }

            }
            HERE:

            if (strcmp(keys[i], aos_field_table[j].key) == 0) {

                aos_field[i].offset = aos_field_table[j].offset;
                aos_field[i].size = aos_field_table[j].size;
                aos_field[i].type = aos_field_table[j].type;

            }

        }

    }

    field_count = complex_field_table_fields_count(soa_field_table);
    for (unsigned int i = 0; i < keys_count; i++) {

        for (unsigned int j = 0; j < field_count; j++) {
        
            if (strcmp(keys[i], soa_field_table[j].key) == 0) {

                soa_field[i].offset = soa_field_table[j].offset;
                soa_field[i].size = soa_field_table[j].size;
                soa_field[i].type = soa_field_table[j].type;
                
            }

        }

    }

    size_t obj_size = complex_field_table_obj_type_size(aos_field_table);
    char *obj = (char *)aos;
    for (unsigned int i = 0; i < obj_count; i++) {

        for (unsigned int j = 0; j < keys_count; j++) {

            size_t aos_offset = aos_field[j].offset;
            //size_t aos_size = aos_field[j].size;
            enum Type src_type = aos_field[j].type;
            size_t soa_offset = soa_field[j].offset;
            size_t soa_size = soa_field[j].size;
            enum Type dest_type = soa_field[j].type;
            void *dest = (char *)soa[soa_offset / sizeof(void *)] + soa_size * i;
            void *src = &obj[obj_size * i + aos_offset];
            convert_type(dest, src, dest_type, src_type);

        }

    }

}


#define MAX_ITER 100

unsigned int complex_field_table_fields_count(const ComplexFieldEntry *field_table) {

    if (field_table == NULL) {
        fprintf(stderr, "at function complex_field_table_fields_count, field_table is NULL");
        return 0;
    }
    _Bool found_sentinel_field = false;
    unsigned int iterations = 0;
    unsigned int idx = 0;
    unsigned int field_table_count = 0;
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
        fprintf(stderr, "At file field_entry.c function field_table_fields_count, the field table passed as args had either one or zero fields\n");
        exit(EXIT_FAILURE);

    }

    field_table_count = (field_table_count == 0) ? 0 : field_table_count - 1;
    return field_table_count;

}

size_t complex_field_table_obj_type_size(const ComplexFieldEntry *field_table) {

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

unsigned int field_table_check_if_keys_exist(const ComplexFieldEntry *field_table, const char **keys, const unsigned int keys_count) {
        
    unsigned int match_count = 0;
    for (unsigned int i = 0; i < keys_count; i++) {

        const _Bool match = recursive_field_table_key_search(field_table, keys[i]);
        match_count += match;
        if (!match) printf("this key -> '%s' doesn't exist in the field table\n", keys[i]);

    }

    const unsigned int mismatch_count = keys_count - match_count;
    if (mismatch_count != 0) fprintf(stderr, "there are %u key(s) that don't exist in field table\n", mismatch_count);
    return mismatch_count;

}

static _Bool recursive_field_table_key_search(const ComplexFieldEntry *field_table, const char *key) {

    static unsigned int recursive_iters = 0;
    recursive_iters++;
    #define MAX_RECURSIVE_ITERS 5U
    if (recursive_iters <= MAX_RECURSIVE_ITERS) {
        
        const unsigned int fields_count = complex_field_table_fields_count(field_table);
        for (unsigned int i = 0; i < fields_count; i++) {

            _Bool match = false;
            if (field_table[i].type == STRUCT) {
                if (field_table[i].struct_field_table == NULL) {
                    fprintf(stderr, "at function recursive_field_table_key_search, struct_field_table is NULL\n");
                    printf("field table entry is at i = %u, and key is %s\n", i, field_table[i].key);
                    continue;
                }
                match = recursive_field_table_key_search(field_table[i].struct_field_table, key);
            } else {
                match = (strcmp(field_table[i].key, key) == 0);
            }

            if (match) {
                recursive_iters--;
                return true;
            }
            
        }
    
        recursive_iters--;
        return false;
    
    } else {
        fprintf(stderr, "function recursive_field_table_key_search reached max recursion limit of %u\n", MAX_RECURSIVE_ITERS);
    }
    #undef MAX_RECURSIVE_ITERS

    recursive_iters--;
    return false;

}

void field_table_match_keys_order(const ComplexFieldEntry *field_table, char **keys, unsigned int keys_count) {

    unsigned int fields_count = complex_field_table_fields_count(field_table);

    signed int tmp[fields_count];
    memset(tmp, -1, sizeof(tmp));

    for (unsigned int i = 0; i < fields_count; i++) {

        for (unsigned int j = 0; j < keys_count; j++) {

            if (strcmp(field_table[i].key, keys[j]) == 0) {
                tmp[i] = (signed int)j;
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

static void convert_type(void *dest, const void *src, enum Type dest_type, enum Type src_type) {

    switch (src_type) {
        
        case BOOL:    store_unsigned(dest, dest_type, *(const _Bool *)src);          break;
        case U_CHAR:  store_unsigned(dest, dest_type, *(const unsigned char *)src);  break;
        case S_CHAR:  store_signed  (dest, dest_type, *(const signed char *)src);    break;
        case U_SHORT: store_unsigned(dest, dest_type, *(const unsigned short *)src); break;
        case S_SHORT: store_signed  (dest, dest_type, *(const short *)src);          break;
        case U_INT:   store_unsigned(dest, dest_type, *(const unsigned int *)src);   break;
        case S_INT:   store_signed  (dest, dest_type, *(const int *)src);            break;
        case U_LONG:  store_unsigned(dest, dest_type, *(const unsigned long *)src);  break;
        case S_LONG:  store_signed  (dest, dest_type, *(const long *)src);           break;
        case FLOAT:   store_double  (dest, dest_type, *(const float *)src);          break;
        case DOUBLE:  store_double  (dest, dest_type, *(const double *)src);         break;
        default: break;

    }

}

static long long load_as_signed(const void *src, enum Type t) {

    switch (t) {

        case BOOL:    return *(const _Bool *)src;
        case U_CHAR:  return *(const unsigned char *)src;
        case S_CHAR:  return *(const signed char *)src;
        case U_SHORT: return *(const unsigned short *)src;
        case S_SHORT: return *(const short *)src;
        case U_INT:   return *(const unsigned int *)src;
        case S_INT:   return *(const int *)src;
        case U_LONG:  return *(const unsigned long *)src;
        case S_LONG:  return *(const long *)src;
        default:      return 0;

    }

}

static double load_as_double(const void *src, enum Type t) {

    switch (t) {

        case FLOAT:   return *(const float *)src;
        case DOUBLE:  return *(const double *)src;
        default:      return (double)load_as_signed(src, t);

    }

}

void subtract_type(void *dest, const void *src1, const void *src2,
                          enum Type dest_type, enum Type src1_type, enum Type src2_type) {

    if (src1_type == NOT_DEFINED || src2_type == NOT_DEFINED) return;

    if (src1_type == FLOAT || src1_type == DOUBLE || src2_type == FLOAT || src2_type == DOUBLE) {

        store_double(dest, dest_type, load_as_double(src1, src1_type) - load_as_double(src2, src2_type));

    } else {

        store_signed(dest, dest_type, load_as_signed(src1, src1_type) - load_as_signed(src2, src2_type));

    }

}

static void store_signed(void *dest, enum Type t, long long v) {

    switch (t) {

        case BOOL:    *(_Bool *)dest          = (_Bool)v;          break;
        case U_CHAR:  *(unsigned char *)dest  = (unsigned char)v;  break;
        case S_CHAR:  *(signed char *)dest    = (signed char)v;    break;
        case U_SHORT: *(unsigned short *)dest = (unsigned short)v; break;
        case S_SHORT: *(short *)dest          = (short)v;          break;
        case U_INT:   *(unsigned int *)dest   = (unsigned int)v;   break;
        case S_INT:   *(int *)dest            = (int)v;            break;
        case U_LONG:  *(unsigned long *)dest  = (unsigned long)v;  break;
        case S_LONG:  *(long *)dest           = (long)v;           break;
        case FLOAT:   *(float *)dest          = (float)v;          break;
        case DOUBLE:  *(double *)dest         = (double)v;         break;
        default: break;

    }

}

static void store_unsigned(void *dest, enum Type t, unsigned long long v) {

    switch (t) {

        case BOOL:    *(_Bool *)dest          = (_Bool)v;          break;
        case U_CHAR:  *(unsigned char *)dest  = (unsigned char)v;  break;
        case S_CHAR:  *(signed char *)dest    = (signed char)v;    break;
        case U_SHORT: *(unsigned short *)dest = (unsigned short)v; break;
        case S_SHORT: *(short *)dest          = (short)v;          break;
        case U_INT:   *(unsigned int *)dest   = (unsigned int)v;   break;
        case S_INT:   *(int *)dest            = (int)v;            break;
        case U_LONG:  *(unsigned long *)dest  = (unsigned long)v;  break;
        case S_LONG:  *(long *)dest           = (long)v;           break;
        case FLOAT:   *(float *)dest          = (float)v;          break;
        case DOUBLE:  *(double *)dest         = (double)v;         break;
        default: break;

    }

}

static void store_double(void *dest, enum Type t, double v) {

    switch (t) {
    
        case BOOL:    *(_Bool *)dest          = (_Bool)v;          break;
        case U_CHAR:  *(unsigned char *)dest  = (unsigned char)v;  break;
        case S_CHAR:  *(signed char *)dest    = (signed char)v;    break;
        case U_SHORT: *(unsigned short *)dest = (unsigned short)v; break;
        case S_SHORT: *(short *)dest          = (short)v;          break;
        case U_INT:   *(unsigned int *)dest   = (unsigned int)v;   break;
        case S_INT:   *(int *)dest            = (int)v;            break;
        case U_LONG:  *(unsigned long *)dest  = (unsigned long)v;  break;
        case S_LONG:  *(long *)dest           = (long)v;           break;
        case FLOAT:   *(float *)dest          = (float)v;          break;
        case DOUBLE:  *(double *)dest         = v;                 break;
        default: break;
    
    }

}