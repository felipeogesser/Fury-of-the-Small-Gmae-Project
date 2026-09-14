#ifndef ANIMATION_H
#define ANIMATION_H

#include "unit_internal.h"
#include "field_entry.h"
#include "general_types.h"
#include <SDL2/SDL.h>

void animation_init(void);
void animation_input(SDL_Event *e);
void animation_update(
    void *object,
    size_t sizeof_obj,
    unsigned int obj_count,
    const FieldEntry *field_table,
    signed int anim_field,
    signed int sprite_field);
//void animation_render(void);
void animation_destroy(void);

//void battleplan_animation_update(General *general);// temp

#endif