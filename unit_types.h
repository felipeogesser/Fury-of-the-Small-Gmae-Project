#ifndef UNIT_TYPES_H
#define UNIT_TYPES_H

#include "field_entry_types.h"

typedef struct Unit Unit;

enum UnitFields {
    U_FIELD_UNKNOWN = -1,
    U_ANIM_FIELD,
    U_SPRITE_FIELD,
    U_ID_FIELD,
};

extern const FieldEntry *const unit_field_table;

#endif