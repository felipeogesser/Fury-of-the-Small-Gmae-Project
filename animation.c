#include "animation.h"
#include "animation_internal.h"
#include "engine_internal.h"
#include "field_entry_internal.h"
#include "game_state_internal.h"
#include "sprites_internal.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <stddef.h>
#include <stdbool.h>

#define ANIMATION_FPS 10
#define FRAME_DURATION (1000 / ANIMATION_FPS)

static Uint32 last_frame_time = 0;
static _Bool change_animation = false;
static enum Animation new_animation = 0;
static bool advance_frame = false;

void animation_init(void) {

    if (IMG_Init(IMG_INIT_PNG) == -1) {
        SDL_Log("IMG_Init failed");
    }

}

void animation_tick(void) {
    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_frame_time >= FRAME_DURATION) {
        last_frame_time = current_time;
        advance_frame = true;
    } else {
        advance_frame = false;
    }
}

void animation_update(
    void *object,
    const size_t sizeof_obj,
    const unsigned int obj_count,
    const FieldEntry *const field_table,
    const signed int anim_field,
    const signed int sprite_field) {

    if (!advance_frame) return;

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