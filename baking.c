#include "baking.h"
#include "calculateEntityQuadrant.h"
#include "engine.h"
#include "grids.h"
#include "loadArmies.h"
#include "mapMaker.h"
#include "maps.h"
#include "player.h"
#include "quadrant.h"
#include "windowSettings.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// prototypes
//static void bake_map(void);
//static void bake_player(void);
static void bake_game_state(void);
//static void bake_armies(void);
//static void bake_grids(void);
//static void bake_entity_quad(void);

// this file still mixes inits with baking. will change later

/*static void bake_map(void) {
    
    make_maps();
    engine.map = get_map(map_id[0]);

}*/

/*static void bake_player(void) { // create_player. isso tem q sair dauqi pq n faz sentido bake player.
    
    init_player();
    unsigned int player_id = create_player(50 ,100, "lipe", 100.0f, 100.0f, 30.0f, 30.0f, true, 4, 150.0f, 2.4f);
    engine.player = get_player(player_id);

}*/

static void bake_game_state(void) {

    engine.game = pass_pointer();

    engine.game->low_LOD_quadrant_size = 64U;

    // offSet moves map, entities, objects, anything other than the player,
    // an amount base on the difference between player spawn and the center window.
    //engine.game->offSetX = engine.player->playerPositionX - WINDOW_SIZE_X / 2 + engine.player->playerDimensionX / 2;
    //engine.game->offSetY = engine.player->playerPositionY - WINDOW_SIZE_Y / 2 + engine.player->playerDimensionY / 2;

}

/*static void bake_armies(void) {
    init_armies_memory_arena();
    load_armies_into_arena();
}

static void bake_grids(void) {
    calculateAmountOfQuadrants();
    init_grids();
    fill_quadrant_data();
}*/

/*void bake_objects(GameState *game) {
    make_objects();

    unsigned int y;

    for (unsigned int i = 0; i < MAX_OBJECTS; i++) {
        if (object_id[i] < 0) break;
        Object *obj = get_object(object_id[i]);
        if (obj == NULL) break;
        game->objects_created_count++;
    }
}*/

/*static void bake_entity_quad(void) {
    initialCheckEntityQuadrant(engine.armies, engine.game, engine.grids);
    renderQuadrantsSetup(engine.armies, engine.game);
}*/

void data_baking(void) {
    srand((unsigned int)time(NULL)); // init global RNG state
    //bake_map();
    //bake_player();
    bake_game_state();
    //bake_armies();
    //bake_grids();
    //bake_entity_quad();
}