#include "inventory.h"
#include "inventory_internal.h"
#include "animation_types.h"
#include "engine_internal.h"
#include "field_entry.h"
#include "general_internal.h"
#include "json_parser.h"
#include "memory_arena.h"
#include "sprites_internal.h"
#include <stdarg.h>
#include <stdbool.h>

// prototypes
void *search_inventory(
    const unsigned short obj_count,
    const char *obj,
    size_t obj_size,
    size_t alignment,
    const FieldEntry *field_table,
    const size_t args_count,
    ...);

/*JsonGeneral json_general = {
    "id",
    "rarity",
    "hp",
    "vigour",
    "attack",
    "defense",
    "evasion",
    "attack_speed",
    "general_type",
    "battalion_type",
    "units_type",
    "sprite"
}*/

Inventory inventory = {0};

void *inventory_init(void) {

    engine.inventory = &inventory;

    inventory.general_count = 4;

    General *general = search_inventory(
        inventory.general_count,
        "generals",
        sizeof(General),
        _Alignof(General),
        general_field_table,
        13,
        "id",
        "rarity",
        "hp",
        "vigour",
        "attack",
        "defense",
        "evasion",
        "attack_speed",
        "general_type",
        "battalion_type",
        "units_type",
        "anim",
        "sprite"
    );

    inventory.general = general;

    for (unsigned int i = 0; i < inventory.general_count; i++) {

        general[i].render = true;
        general[i].anim.animation = IDLE;
        general[i].anim.frames_count = engine.sprite_pack->sprite[general[i].sprite.type][IDLE].frames_count;
        general[i].anim.current_frame =(unsigned char)(rand() % general[i].anim.frames_count);

    }

    return general;

}

void *search_inventory(
    const unsigned short obj_count,
    const char *obj,
    size_t obj_size,
    size_t alignment,
    const FieldEntry *field_table,
    const size_t args_count,
    ...) {

    void *memory_p = memory_arena_push(obj_size * obj_count, alignment);

    char *buffer = open_read_close_file("inventory.json");

    if (!buffer) {
        fprintf(stderr, "buffer is null\n");
        exit(EXIT_FAILURE);
    }

    va_list args;
    va_start(args, args_count);

    char *keys[args_count];
    for (unsigned int i = 0; i < args_count; i++) {
        keys[i] = va_arg(args, char *);
    }
    char *p = buffer;
    read_file_and_retrieve_data(memory_p, p, field_table, obj_count, obj, keys, args_count);

    free(buffer);
    
    va_end(args);
    
    return memory_p;

}
