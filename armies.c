#include "armies.h"
#include "armies_internal.h"

/*void set_army_position(void) {

    unsigned int formation_height = battalion->unit_formation_height;
    unsigned int formation_width = ceilf(battalion_size / battalion->unit_formation_height);
    unsigned int battalion_pos_x = battalion->initial_map_placement_x;
    unsigned int battalion_pos_y = battalion->initial_map_placement_y;
    unsigned int padding_x = battalion->padding_between_units_x;
    unsigned int padding_y = battalion->padding_between_units_y;

    initial_map_placement_x=50
initial_map_placement_y=50
padding_between_battalions_x=10
padding_between_battalions_y=10
    unsigned int idx = 0;
    for (unsigned int i = 0; i < formation_width; i++) {

        for (unsigned int j = 0; j < formation_height; j++) {

            unsigned int x = battalion_pos_x + padding_x + (unit[idx].dimensionX * i);
            unsigned int y = battalion_pos_y + padding_y + (unit[idx].dimensionY * j);
            unit[idx].positionX = x;
            unit[idx].positionY = y;
            idx++;
            if (idx == battalion_size) return;
            
        }

    }

}*/