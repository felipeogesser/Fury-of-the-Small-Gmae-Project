#include "animation.h"
#include "animation_internal.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "entities_internal.h"
#include "game_state_internal.h"
#include "general_internal.h"
#include "engine_internal.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <stddef.h>
#include <stdbool.h>

#define ANIMATION_FPS 3
#define FRAME_DURATION (1000 / ANIMATION_FPS)
#define SPRITE_FRAME_END 7 // each png has 8 frames
Animation animation = {0};
AnimationState animationState = {0};

SDL_Surface *surface = NULL;
SDL_Texture *texture = NULL;

static Uint32 last_frame_time = 0;
static unsigned char sprite_frame = 0;
static _Bool change_sprite_state = false;

enum SpriteState new_sprite_state;

void animation_init(void) {

    if (IMG_Init(IMG_INIT_PNG) == -1) {
        SDL_Log("IMG_Init failed");
    }

    engine.animation = &animation;
    engine.animationState = &animationState;
    

    SDL_Renderer *renderer = engine.renderer;
    surface = IMG_Load("assets/sprites/Warrior/Warrior_Idle.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = NULL;

    
}

void animation_input(SDL_Event *e) {

    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_RIGHT) {

        change_sprite_state = true;
        new_sprite_state = RUN;

    }

}

void animation_update(void) {

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_frame_time >= FRAME_DURATION) {

        last_frame_time = current_time;

        if (change_sprite_state && sprite_frame == 0) {
            animationState.sprite_state = new_sprite_state;
            change_sprite_state = false;
        }

        animationState.sprite_frame = sprite_frame;

        /*if (++sprite_frame == sprite_frame_end) {
            sprite_frame = 0;
        }*/
        sprite_frame++;
        if (!(sprite_frame & SPRITE_FRAME_END)) {
            sprite_frame = 0;
        }

    }

}

void animation_render(void) {

    SDL_Renderer *renderer = engine.renderer;

    unsigned int png_width = 1536;
    unsigned int png_height = 192;
    unsigned int sprite_frame_width = png_width / 8;
    unsigned int sprite_frame_height = png_height;

    SDL_Rect sprite_slice = {
        (signed int)(sprite_frame_width * sprite_frame),
        0,
        (signed int)sprite_frame_width,
        (signed int)sprite_frame_height
    };
    
    SDL_Rect sprite_screen_position = {
        (signed int)engine.armies->army->general->battalions->entities->positionX - (signed int)engine.game->offSetX,
        (signed int)engine.armies->army->general->battalions->entities->positionY - (signed int)engine.game->offSetY,
        (signed int)engine.armies->army->general->battalions->entities_screen_width,
        (signed int)engine.armies->army->general->battalions->entities_screen_height
    };
    
    const SDL_Rect *rect1 = &sprite_slice;
    const SDL_Rect *rect2 = &sprite_screen_position;

    SDL_RenderCopy(renderer, texture, rect1, rect2);

}

void animation_destroy(void) {
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
}

