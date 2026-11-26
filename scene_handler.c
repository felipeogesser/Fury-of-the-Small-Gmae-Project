#include "scene_handler.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "help_overlay.h"
#include "scene_registry.h"
#include "tutorial.h"

static _Bool has_help_overlay(enum Scene scene) {

    return scene == STAGE_MAP || scene == BATTLEPLAN || scene == BATTLEFIELD;

}

void scene_init(enum Scene scene) {

    if (engine.window_running) {

        engine.game->scene_state.scene = scene;
        
        scene_registry[scene].init();

    }

}

void scene_input(SDL_Event *e) {

    if (engine.window_running) {

        enum Scene current_scene = engine.game->scene_state.scene;

        if (help_overlay_is_open() && help_overlay_input(e)) return;
        if (tutorial_input(current_scene, e)) return;
        if (has_help_overlay(current_scene) && help_overlay_input(e)) return;

        scene_registry[current_scene].input(e);

    }

}

void scene_update(float dt) {

    if (engine.window_running) {

        enum Scene current_scene = engine.game->scene_state.scene;

        scene_registry[current_scene].update(dt);

    }

}

void scene_render(void) {

    SDL_Renderer *renderer = engine.renderer;

    if (engine.window_running && engine.render == ENABLED) {

        enum Scene current_scene = engine.game->scene_state.scene;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        scene_registry[current_scene].render();

        if (has_help_overlay(current_scene)) help_overlay_render();
        tutorial_render(current_scene);

        SDL_RenderPresent(renderer);

    }

//    SDL_RenderClear(renderer);

}

void scene_destroy(void) {

    enum Scene current_scene = engine.game->scene_state.scene;

    scene_registry[current_scene].destroy();

}

void scene_switch(enum Scene next_scene) {

    if (engine.window_running) {

        enum Scene current_scene = engine.game->scene_state.scene;

        scene_registry[current_scene].destroy();

        engine.game->scene_state.scene = next_scene;

        help_overlay_close();
        scene_registry[next_scene].init();
        tutorial_scene_entered(next_scene);

        float dt = engine.game->delta;
        scene_registry[next_scene].update(dt);

    }

}