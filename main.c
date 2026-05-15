#include "baking.h"
#include "gameLoop.h"
#include "init.h"
#include "shutdown.h"
#include "window.h"

int main(void) {

    init();
    
    data_baking();
    
    create_window();

    game_loop();

    shutdown();

}
