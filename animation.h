#ifndef ANIMATION_H
#define ANIMATION_H

#include "field_entry_types.h"
#include <stddef.h>

void animation_init(void);
void animation_update(
    void *object,
    size_t sizeof_obj,
    unsigned int obj_count,
    const FieldEntry *field_table,
    signed int anim_field,
    signed int sprite_field);
void animation_destroy(void);

#endif