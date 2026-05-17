#include "scene_registry.h"
#include "main_menu.h"
#include "scenes.h"

#define SCENE(init_fn, input_fn, update_fn, render_fn, destroy_fn) \
{                                                         \
    .init = init_fn,                                      \
    .input = input_fn,                                    \
    .update = update_fn,                                  \
    .render = render_fn,                                  \
    .destroy = destroy_fn                                 \
}

void void_function(void) {
    return;
}

const struct SceneCallbacks scene_registry[SCENE_COUNT] = {
    
    [MAIN_MENU] = SCENE(
            main_menu_init,
            main_menu_input,
            void_function,
            main_menu_render,
            main_menu_destroy
    )/*,

    [BATTLEFIELD] = SCENE(
            battlefield_init,
            battlefield_input,
            battlefield_update,
            battlefield_render,
            battlefield_destroy
    )*/
    
};