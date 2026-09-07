#include "battalion.h"
#include "battalion_types.h"
#include "battalion_internal.h"
#include "field_entry.h"
#include <math.h>

// private prototypes
//static void set_battalion_position(Battalion *battalion, unsigned int x, unsigned int y);
static void set_battalion_formation(Battalion *battalion, unsigned int formation_width, unsigned int formation_height, unsigned int padding_between_units_x, unsigned int padding_between_units_y);
//static void set_battalion_dimension(Battalion *battalion, unsigned int battalion_width, unsigned int battalion_height);

#define OFFSET_OF(type, member) ((size_t) &(((type *)0)->member))
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))

const FieldEntry battalion_field_table[] = {
    { "padding_between_units_x",    OFFSET_OF(Battalion, padding_between_units_x),  SIZE_OF(Battalion, padding_between_units_x) },
    { "padding_between_units_y",    OFFSET_OF(Battalion, padding_between_units_y),  SIZE_OF(Battalion, padding_between_units_y) },
    { "unit_formation_width",       OFFSET_OF(Battalion, unit_formation_width),     SIZE_OF(Battalion, unit_formation_width) },
    { "unit_formation_height",      OFFSET_OF(Battalion, unit_formation_height),    SIZE_OF(Battalion, unit_formation_height) },
    { "unit_screen_width",          OFFSET_OF(Battalion, unit_screen_width),        SIZE_OF(Battalion, unit_screen_width) },
    { "unit_screen_height",         OFFSET_OF(Battalion, unit_screen_height),       SIZE_OF(Battalion, unit_screen_height) },
    { "obj_size",                   0,                                              sizeof(Battalion) },
};
const size_t battalion_field_table_count =
    sizeof(battalion_field_table) / sizeof(battalion_field_table[0]);
#undef OFFSET_OF
#undef SIZE_OF


void init_battalion(Battalion *battalion, unsigned int battalion_size) {
    
    battalion->unit_count = battalion_size;

    //unsigned int battalion_width = 50;
    //unsigned int battalion_height = 100;
    unsigned int formation_height = 10;
    unsigned int formation_width = ceilf(battalion_size / formation_height);
    unsigned int padding_between_units_x = 5;
    unsigned int padding_between_units_y = 5;

    set_battalion_formation(
        battalion,
        formation_width,
        formation_height,
        padding_between_units_x,
        padding_between_units_y
    );

    //set_battalion_dimension(battalion, battalion_width, battalion_height);

}

static void set_battalion_formation(
    Battalion *battalion,
    unsigned int formation_width,
    unsigned int formation_height,
    unsigned int padding_between_units_x,
    unsigned int padding_between_units_y) {

    battalion->unit_formation_width = formation_width;
    battalion->unit_formation_height = formation_height;
    battalion->padding_between_units_x = padding_between_units_x;
    battalion->padding_between_units_y = padding_between_units_y;

}

/*static void set_battalion_dimension(Battalion *battalion, unsigned int battalion_width, unsigned int battalion_height) {

    battalion->area_width = battalion_width;
    battalion->area_height = battalion_height;

}*/