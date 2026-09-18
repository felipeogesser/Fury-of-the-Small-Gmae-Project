#include "baking.h"
#include "game_loop.h"
#include "game_shutdown.h"
#include "init.h"

int main(void) {

    init();
    
    data_baking();
    
    game_loop();

    game_shutdown();

}
