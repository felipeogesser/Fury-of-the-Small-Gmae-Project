#include "army_health.h"

float army_health_on_field(const Table *tables, unsigned int battalion_count, unsigned int battalion_size) {

    const UnitDynamicTable *units = &tables->unit_tables.friendly_dynamic;
    const GeneralDynamicTable *generals = &tables->general_tables.friendly_dynamic;
    float total = 0.0f;

    for (unsigned int i = 0; i < battalion_count * battalion_size; i++) {
        if (units->is_present[i] && units->hp[i] > 0.0f) total += units->hp[i];
    }

    for (unsigned int i = 0; i < battalion_count; i++) {
        if (generals->is_present[i] && generals->hp[i] > 0.0f) total += generals->hp[i];
    }

    return total;

}
