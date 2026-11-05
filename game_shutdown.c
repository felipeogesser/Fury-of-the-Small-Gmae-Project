#include "game_shutdown.h"
#include "engine_cleanup.h"
#include "scene_handler.h"
#include "ui.h"
#include "window.h"

void game_shutdown(void) {

    scene_destroy();
    ui_shutdown();
    destroy_window();
    engine_cleanup_run();

}