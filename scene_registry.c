#include "scene_registry.h"
#include "battlefield.h"
#include "battleplan.h"
#include "main_menu.h"

static void void_function(void);
static void void_function(void) {
    return;
}

const SceneCallbacks scene_registry[SCENE_COUNT] = {

    {
        main_menu_init,
        main_menu_input,
        void_function,
        main_menu_render,
        main_menu_destroy
    },

    {
        battleplan_init,
        battleplan_input,
        battleplan_update,
        battleplan_render,
        battleplan_destroy
    },

    {
        battlefield_init,
        battlefield_input,
        battlefield_update,
        battlefield_render,
        battlefield_destroy
    }

};
