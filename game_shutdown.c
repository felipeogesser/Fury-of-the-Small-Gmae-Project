#include "game_shutdown.h"
#include "engine_cleanup.h"
#include "scene_handler.h"
#include "window.h"

void game_shutdown(void) {

    scene_destroy();
    destroy_window();
    engine_cleanup_run();

}