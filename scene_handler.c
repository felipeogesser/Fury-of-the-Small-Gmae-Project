#include "scene_handler.h"
#include "animation.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "scene_registry.h"

/*void scene_handler_init(void) {

}*/

void scene_init(enum SceneState scene) {

    if (engine.window_running) {
        
        scene_registry[scene].init();

        engine.game->scene_state = scene;

    }
}

void scene_input(SDL_Event *e) {

    if (engine.window_running) {

        enum SceneState current_scene = engine.game->scene_state;

        scene_registry[current_scene].input(e);

    }
}

void scene_update(void) {

    if (engine.window_running) {

        enum SceneState current_scene = engine.game->scene_state;

        scene_registry[current_scene].update();

    }
}

void scene_render(void) {

    if (engine.window_running) {

        enum SceneState current_scene = engine.game->scene_state;

        SDL_Renderer *renderer = engine.renderer;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        scene_registry[current_scene].render();

        SDL_RenderPresent(renderer);

    }
}

void scene_destroy(void) {

    enum SceneState current_scene = engine.game->scene_state;

    scene_registry[current_scene].destroy();

}

void scene_switch(enum SceneState next_scene) {

    if (engine.window_running) {

        enum SceneState current_scene = engine.game->scene_state;

        scene_registry[current_scene].destroy();

        scene_registry[next_scene].init();

        scene_registry[next_scene].update();

        engine.game->scene_state = next_scene;

    }
}