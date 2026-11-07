#include "animation.h"
#include "animation_internal.h"
#include "engine_internal.h"
#include "field_entry_internal.h"
#include "game_clock.h"
#include "game_state_internal.h"
#include "sprites_internal.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <stddef.h>
#include <stdbool.h>

#define ANIMATION_FPS 10
#define FRAME_DURATION (1000 / ANIMATION_FPS)

const ComplexFieldEntry animation_state_complex_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "animation",      OFFSET_OF(AnimationState, animation),      SIZE_OF(AnimationState, animation),      S_INT,       NULL },
    { "frames_count",   OFFSET_OF(AnimationState, frames_count),   SIZE_OF(AnimationState, frames_count),   U_CHAR,      NULL },
    { "current_frame",  OFFSET_OF(AnimationState, current_frame),  SIZE_OF(AnimationState, current_frame),  U_CHAR,      NULL },
    { "obj_size",       0,                                         sizeof(AnimationState),                  NOT_DEFINED, NULL },

    #undef OFFSET_OF
    #undef SIZE_OF

};

static const FieldEntry animation_state_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "animation",      OFFSET_OF(AnimationState, animation),      SIZE_OF(AnimationState, animation)      },
    { "frames_count",   OFFSET_OF(AnimationState, frames_count),   SIZE_OF(AnimationState, frames_count)   },
    { "current_frame",  OFFSET_OF(AnimationState, current_frame),  SIZE_OF(AnimationState, current_frame)  },
    { "obj_size",       0,                                         sizeof(AnimationState)                  },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const FieldEntry *const animation_state_field_table_ptr = animation_state_field_table;

//static _Bool change_animation = false; // maybe remove, not used
//static _Bool advance_frame = false;

void animation_init(void) {

    if (IMG_Init(IMG_INIT_PNG) == -1) {
        SDL_Log("IMG_Init failed");
    }

}

_Bool animation_tick(void) {

    static Uint64 frame_cached = (Uint64)-1;
    static Uint32 last_tick_time = 0;
    static _Bool should_tick_animation = false;

    Uint64 frame = game_clock_frame_count();

    if (frame != frame_cached) {
        frame_cached = frame;
        Uint32 now = SDL_GetTicks();
        should_tick_animation = (now - last_tick_time >= FRAME_DURATION);
        if (should_tick_animation) last_tick_time = now;
    }

    return should_tick_animation;

}

/*static float frame_timer = FRAME_DURATION;
void animation_tick(float dt) {
    frame_timer -= dt;
    if (frame_timer <= 0.0f) {
        frame_timer = FRAME_DURATION;
        advance_frame = true;
    } else {
        advance_frame = false;
    }
}*/

void animation_update(
    void *object,
    const size_t sizeof_obj,
    const unsigned int obj_count,
    const FieldEntry *const field_table,
    const signed int anim_field,
    const signed int sprite_field) {

    static _Bool change_animation = false;
    static enum Animation new_animation = 0;

    if (!animation_tick()) return;

    size_t anim_offset = field_table[anim_field].offset;
    size_t sprite_offset = field_table[sprite_field].offset;

    char *obj = (char *)object;

    for (unsigned int i = 0; i < obj_count; i++) {
        AnimationState *const anim = (AnimationState *)(obj + anim_offset);
        const SpriteInfo *const sprite = (const SpriteInfo *)(obj + sprite_offset);
        obj += sizeof_obj;

        if (anim->frames_count == 0) {
            fprintf(stderr, "Frame count is 0.\n");
            exit(EXIT_FAILURE);
        }

        anim->current_frame =
            (anim->current_frame == anim->frames_count - 1) ?
            0 : anim->current_frame + 1;

        if (change_animation && anim->animation != new_animation) {
            anim->animation = new_animation;
            anim->current_frame = 0;
            anim->frames_count = engine.sprite_pack->sprite[sprite->type][new_animation].frames_count;
        }
    }
}

void animation_destroy(void) {
    
    SDL_Texture *texture = NULL;
    for (unsigned int i = 0; i < SPRITES_COUNT; i++) {
        for (unsigned int j = 0; j < ANIMATION_COUNT; j++) {
            texture = engine.sprite_pack->sprite[i][j].texture;

            if (texture != NULL) {
                SDL_DestroyTexture(texture);
            }
        }
    }

    IMG_Quit();

}

/*
void animation_update(
    void *object,
    size_t sizeof_obj,
    unsigned int obj_count,
    const FieldEntry *field_table,
    signed int anim_field,
    signed int sprite_field) {

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_frame_time >= FRAME_DURATION) {

        last_frame_time = current_time;

        size_t anim_offset = field_table[anim_field].offset;
        size_t sprite_offset = field_table[sprite_field].offset;

        char *obj = (char *)object;

        for (unsigned int i = 0; i < obj_count; i++) {

            AnimationState *anim = (AnimationState *)(obj + anim_offset);
            SpriteInfo *sprite = (SpriteInfo *)(obj + sprite_offset);
            obj += sizeof_obj;

            if (anim->frames_count == 0) {
                fprintf(stderr, "Frame count is 0.\n");
                exit(EXIT_FAILURE);
            }

            anim->current_frame =
                (anim->current_frame == anim->frames_count - 1) ?
                0 : anim->current_frame + 1;

            if (change_animation && anim->animation != new_animation) {
                anim->animation = new_animation;
                anim->current_frame = 0;
                anim->frames_count = engine.sprite_pack->sprite[sprite->type][new_animation].frames_count;
            }

        }

        change_animation = false;

    }

}*/