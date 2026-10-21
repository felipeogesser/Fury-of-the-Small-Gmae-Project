#include "baking.h"
#include "game_loop.h"
#include "game_shutdown.h"
#include "init.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    _Bool is_ai_bypass_disabled = true;
    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "--ai") == 0) is_ai_bypass_disabled = false;
        else fprintf(stderr, "unknown argument '%s', ignored.\n", argv[i]);

    }

    init(is_ai_bypass_disabled);
    
    data_baking();
    
    game_loop();

    game_shutdown();

    return 0;

}
