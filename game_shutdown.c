#include "game_shutdown.h"
#include "engine.h"
#include "scenes.h"
#include "window.h"

void game_shutdown(void) {

    scene_destroy();
    destroy_window();
    engine_cleanup_run();

}