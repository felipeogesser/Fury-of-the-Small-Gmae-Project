#ifndef POSITION_H
#define POSITION_H

#include "field_entry_internal.h"

typedef struct Position Position;

typedef struct Position {

    signed int x, y;

} Position;

extern const ComplexFieldEntry position_field_table[];

#endif