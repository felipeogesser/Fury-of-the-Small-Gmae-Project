#include "baking.h"
#include "game_loop.h"
#include "game_shutdown.h"
#include "init.h"
//#include "window.h"

int main(void) {

    init();
    
    data_baking();
    
    //create_window();

    game_loop();

    game_shutdown();

}
