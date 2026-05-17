#include "scene_handler.h"
#include "engine.h"
#include "scene_registry.h"

/*void scene_handler_init(void) {

}*/

void scene_switch(enum SceneState next_scene) {

    enum SceneState current_scene = engine.game->scene_state;

    scene_registry[current_scene].destroy();

    scene_registry[next_scene].init();

    engine.game->scene_state = next_scene;

}

void scene_input(SDL_Event *e) {

    enum SceneState current_scene = engine.game->scene_state;

    scene_registry[current_scene].input(e);

}

void scene_render(void) {

    enum SceneState current_scene = engine.game->scene_state;

    scene_registry[current_scene].render();

}
