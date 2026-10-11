#include "aos_soa_transformer.h"
#include "field_entry.h"
#include "field_entry_types.h"
#include <stddef.h>

void aos_soa_transform(
    void *aos,
    void **soa,
    const ComplexFieldEntry *aos_field_table,
    const ComplexFieldEntry *soa_field_table,
    size_t obj_count,
    unsigned int args_count, ...) {

        // future fixes: error handling, check for obj_count and soa arrays lengh,
        // if all keys match. make a choice between "copy all keys, copy all possible, copy all but args"

    if (args_count <= 0) {

        fprintf(stderr, "at function aos_soa_transform, args_count is either <= 0\n");
        return NULL;
        
    }

    va_list args;
    va_start(args, args_count);

    unsigned int keys_count = args_count;
    char *keys[keys_count];
    for (unsigned int i = 0; i < keys_count; i++) {

        keys[i] = va_arg(args, char *);

    }
    
    va_end(args);

    unsigned int mismatch_count = 0;
    mismatch_count = field_table_check_if_keys_exist(aos_field_table, &keys, keys_count);
    if (!mismatch_count) {

        exit(EXIT_FAILURE);

    }
    mismatch_count = field_table_check_if_keys_exist(soa_field_table, &keys, keys_count);
    if (!mismatch_count) {

        exit(EXIT_FAILURE);

    }

    field_table_match_keys_order(soa_field_table, &keys, keys_count);

    copy_aos_key_fields_to_soa(
        soa_field_table,
        aos_field_table,
        soa,
        aos,
        obj_count,
        &keys,
        keys_count
    );

}
