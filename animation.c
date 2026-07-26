#include "animation.h"
#include "animation_internal.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "camera.h"
#include "camera_internal.h"
#include "engine_internal.h"
#include "entities_internal.h"
#include "game_state_internal.h"
#include "general_internal.h"
#include "sprites_internal.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <stddef.h>
#include <stdbool.h>

#define ANIMATION_FPS 3
#define FRAME_DURATION (1000 / ANIMATION_FPS)

static Uint32 last_frame_time = 0;
static _Bool change_animation = false;
static enum Animation new_animation = 0;

void animation_init(void) {

    if (IMG_Init(IMG_INIT_PNG) == -1) {
        SDL_Log("IMG_Init failed");
    }

    //engine.animationState = &animationState;

    /*for (unsigned int i = 0; i < engine.game->entities_created_count; i++) {

        unsigned int unit_type = engine.armies->army->general->units_type;
        entities[i].sprite->texture = sprite_pack->sprite[unit_type][IDLE];

    }*/

}

void animation_input(SDL_Event *e) {

    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_RIGHT) {

        change_animation = true;
        new_animation = RUN;

    }

}

void animation_update(void) {

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_frame_time >= FRAME_DURATION) {

        last_frame_time = current_time;

        Entity *entities = engine.armies->army->general->battalions->entities;

        for (unsigned int i = 0; i < engine.game->entities_created_count; i++) {
            
            if (change_animation) {
                entities[i].animation = new_animation;
            }
            entities[i].sprite_current_frame =
                (entities[i].sprite_current_frame == entities[i].sprite_frames_count) ?
                0 : entities[i].sprite_current_frame + 1;
        }
    }

}

void animation_render(void) {

    SDL_Renderer *renderer = engine.renderer;
    Battalion *battalions = engine.armies->army->general->battalions;
    Entity *entities = engine.armies->army->general->battalions->entities;

    for (unsigned int i = 0; i < engine.armies->army->battalion_count; i++) {

        unsigned int units_type = engine.armies->army->general[i].units_type;

        for (unsigned int j = 0; j < engine.game->entities_created_count; j++) {

            signed int png_width = sprite_pack.sprite[units_type][entities[j].animation].width;
            signed int png_height = sprite_pack.sprite[units_type][entities[j].animation].height;
            signed int sprite_frame_width = png_width / sprite_pack.sprite[units_type][entities[j].animation].frames_count;
            signed int sprite_frame_height = png_height;

            SDL_Rect sprite_slice = {
                (sprite_frame_width * entities[j].sprite_current_frame),
                0,
                sprite_frame_width,
                sprite_frame_height
            };

            SDL_Rect sprite_position = {
                (signed int)entities[j].positionX - 10,
                (signed int)entities[j].positionY - 10,
                (signed int)battalions->entities_screen_width,
                (signed int)battalions->entities_screen_height
            };
            
            camera_world_to_screen(&sprite_position);

            const SDL_Rect *rect1 = &sprite_slice;
            const SDL_Rect *rect2 = &sprite_position;

            SDL_Texture *texture = sprite_pack.sprite[units_type][entities[j].animation].texture;
            SDL_RenderCopy(renderer, texture, rect1, rect2);
        
        }

    }

}

void animation_destroy(void) {
    
    SDL_Texture *texture = NULL;
    for (unsigned int i = 0; i < SPRITES_COUNT; i++) {
        for (unsigned int j = 0; j < ANIMATION_COUNT; j++) {
            texture = sprite_pack.sprite[i][j].texture;

            if (texture != NULL) {
                SDL_DestroyTexture(texture);
            }
        }
    }

}
