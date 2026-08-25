#include "battalion.h"
#include "battalion_types.h"
#include "battalion_internal.h"
#include <math.h>

// private prototypes
static void set_battalion_xy_position(Battalion *battalion, unsigned int x, unsigned int y);
static void set_battalion_formation(Battalion *battalion, unsigned int formation_width, unsigned int formation_height, unsigned int padding_between_units_x, unsigned int padding_between_units_y);
static void set_battalion_dimension(Battalion *battalion, unsigned int battalion_width, unsigned int battalion_height);

void init_battalion(Battalion *battalion, unsigned int battalion_size, unsigned int x, unsigned int y) {
    
    battalion->unit_count = battalion_size;

    unsigned int padding_left = 100;
    //unsigned int padding_right = 100;
    unsigned int padding_top = 50;
    //unsigned int padding_bottom = 50;
    unsigned int battalion_width = 50;
    unsigned int battalion_height = 100;
    unsigned int formation_height = 10;
    unsigned int formation_width = ceilf(battalion_size / formation_height);
    unsigned int padding_between_battalions_x = 20;
    unsigned int padding_between_battalions_y = 10;
    unsigned int padding_between_units_x = 10;
    unsigned int padding_between_units_y = 20;
    unsigned int grid_x_relative_to_map = padding_left + x * battalion_width + padding_between_battalions_x;
    unsigned int grid_y_relative_to_map = padding_top + y * battalion_height + padding_between_battalions_y;

    set_battalion_xy_position(battalion, grid_x_relative_to_map, grid_y_relative_to_map);

    set_battalion_formation(
        battalion,
        formation_width,
        formation_height,
        padding_between_units_x,
        padding_between_units_y
    );

    set_battalion_dimension(battalion, battalion_width, battalion_height);

}

static void set_battalion_xy_position(Battalion *battalion, unsigned int x, unsigned int y) {

    battalion->initial_map_placement_x = x;
    battalion->initial_map_placement_y = y;

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

static void set_battalion_dimension(Battalion *battalion, unsigned int battalion_width, unsigned int battalion_height) {

    battalion->area_width = battalion_width;
    battalion->area_height = battalion_height;

}