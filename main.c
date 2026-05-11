#include "baking.h"
#include "gameLoop.h"
#include "window.h"

int main(void) {

    data_baking();
    
    create_window();

    game_loop();

}
