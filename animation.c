#include "animation.h"
#include "animation_internal.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "camera.h"
#include "camera_internal.h"
#include "engine_internal.h"
#include "unit_internal.h"
#include "game_state_internal.h"
#include "general_internal.h"
#include "inventory_internal.h"
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

void animation_init(void) {

    if (IMG_Init(IMG_INIT_PNG) == -1) {
        SDL_Log("IMG_Init failed");
    }

}

static unsigned int click_count = 0;
void animation_input(SDL_Event *e) {

    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_RIGHT) {

        change_animation = true;
        new_animation = RUN;

    }

/*if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_LEFT) {

        change_animation = true;
        new_animation = IDLE;

    }*/

    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_LEFT) {

        Unit *unit = engine.armies->army->general->battalions->unit;

        change_animation = true;
        if (click_count == 0) {
            new_animation = IDLE;
        } else if (click_count == 1) {
            new_animation = RUN;
        } else if (click_count == 2) {
            new_animation = ATTACK;
        } else {
            click_count = 0;
            new_animation = IDLE;
        }

        click_count++;
        for (unsigned int i = 0; i < engine.game->unit_created_count; i++) {
        
            unit[i].sprite.type = RAFA;
        }

    }

}

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

}

/*void animation_render(
    void *object,
    size_t sizeof_obj,
    unsigned int obj_count,
    FieldEntry *field_table,
    signed int anim_field,
    signed int sprite_field) {

    SDL_Renderer *renderer = engine.renderer;
    Battalion *battalions = engine.armies->army->general->battalions;
    Unit *unit = engine.armies->army->general->battalions->unit;

        size_t anim_offset = field_table[anim_field].offset;
        size_t sprite_offset = field_table[sprite_field].offset;
        size_t pos_x
        size_t pos_y

        char *obj = (char *)object;

    for (unsigned int i = 0; i < obj_count; i++) {

        AnimationState *anim = (AnimationState *)(obj + anim_offset);
        SpriteInfo *spr = (SpriteInfo *)(obj + sprite_offset);
        obj += sizeof_obj;

        enum Animation animation = anim->animation;
        enum Sprites type = spr->type;
        
        Sprite *sprite = &engine.sprite_pack->sprite[type][animation];

        signed int png_width = sprite->width;
        signed int png_height = sprite->height;
        signed int sprite_frame_width = png_width / sprite->frames_count;
        signed int sprite_frame_height = png_height;

        SDL_Rect sprite_slice = {
            (sprite_frame_width * anim->current_frame),
            0,
            sprite_frame_width,
            sprite_frame_height
        };

        SDL_Rect sprite_position = {
            (signed int)unit[i].positionX,
            (signed int)unit[i].positionY,
            (signed int)sprite_frame_width,
            (signed int)sprite_frame_height
        };
        
        camera_world_to_screen(&sprite_position);

        const SDL_Rect *rect1 = &sprite_slice;
        const SDL_Rect *rect2 = &sprite_position;

        SDL_Texture *texture = sprite->texture;
        SDL_RenderCopy(renderer, texture, rect1, rect2);
    
    }

}*/

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

}





//////

/*void battleplan_animation_update(General *general);
void battleplan_animation_update(General *general) {

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_frame_time >= FRAME_DURATION) {

        last_frame_time = current_time;

        for (unsigned int i = 0; i < engine.inventory->general_count; i++) {

            if (general[i].anim.frames_count == 0) {
                fprintf(stderr, "Frame count is 0.\n");
                exit(EXIT_FAILURE);
            }


            general[i].anim.current_frame =
                (general[i].anim.current_frame == general[i].anim.frames_count - 1) ?
                0 : general[i].anim.current_frame + 1;

            if (change_animation && general[i].anim.animation != new_animation) {
                general[i].anim.animation = new_animation;
                general[i].anim.current_frame = 0;
                general[i].anim.frames_count = engine.sprite_pack->sprite[general[i].sprite][new_animation].frames_count;
            }
        }

        change_animation = false;

    }

}*/