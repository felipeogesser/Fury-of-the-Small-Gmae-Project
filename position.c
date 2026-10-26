#include "position.h"
#include "field_entry_internal.h"
#include <stddef.h>

#define OFFSET_OF(type, member) offsetof(type, member)
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))
#define ELEMENT_SIZE_OF(type, member) (sizeof(*(((type *)0)->member)))

const ComplexFieldEntry position_field_table[] = {

    { "x",           OFFSET_OF(Position, x),           SIZE_OF(Position, x),           S_INT, NULL },
    { "y",     OFFSET_OF(Position, y),     SIZE_OF(Position, y),     S_INT, NULL },
    { "obj_size", 0, sizeof(Position), STRUCT, NULL }

};

#undef OFFSET_OF
#undef SIZE_OF
#undef ELEMENT_SIZE_OF