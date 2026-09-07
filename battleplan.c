#include "battleplan.h"
#include "battleplan_internal.h"
#include "animation.h"
#include "animation_types.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "general_internal.h"
#include "inventory.h"
#include "inventory_internal.h"
#include "memory_arena.h"
#include "scene_handler.h"
#include "scenes.h"
#include "sprites.h"
#include "sprites_internal.h"
#include "window_settings.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

enum Origin {
    NO_ORIGIN = -1,
    GRID,
    DRAWER
};

typedef struct DragPayload {

    General *general;
    enum Origin origin;
    signed int grid_cell_x;
    signed int grid_cell_y;

} DragPayload;

DragPayload drag_payload = {0};

// private prototypes
static void booleans_init(void);
static void variables_init(void);
static void set_drawer_slot_generals_index(General *general);
static void set_minimal_general_sprite_and_animation_data(General *general);
static void handle_mouse_left_button(void);
static signed int check_if_dragging_general_out_of_grid(void);
static void check_if_dragging_general_out_of_drawer(void);
static void on_mouse_left_click(void);
static void on_left_mouse_release(void);
static void clear_payload(DragPayload *payload);
static void battleplan_render_deployment_area(SDL_Renderer *renderer);
static void battleplan_render_general_drawer(SDL_Renderer *renderer);
static void render_drawer_generals(SDL_Renderer *renderer);
static void render_grid_generals(SDL_Renderer *renderer);
static void render_dragged_general(SDL_Renderer *renderer);
static void open_general_drawer(void);
static void close_general_drawer(void);
static float ease_out_elastic(float x);
//static float ease_in_elastic(float x);
static void place_general_on_grid(General **grid_cell, General **general_to_place);
static void swap_generals(General **general_in_grid, DragPayload *payload);
static void insert_general_into_drawer(General **general_to_insert);
static void save_battleplan_placement(void);
static _Bool grid_is_empty(void);

static Battleplan battleplan = {0};

static TTF_Font *battleplan_font = NULL;

static SDL_Color battleplan_white = {255, 255, 255, 255};

static SDL_Surface *battleplan_surface = NULL; 

static SDL_Texture *battleplan_message = NULL;

static SDL_Rect drawer_handle = {0};
static SDL_Rect drawer = {0};

static _Bool left_mouse_pressed = false;
static _Bool holding_mouse = false;
static _Bool open_drawer = false;
static _Bool close_drawer = false;
static _Bool drawer_opened = false;
static _Bool drawer_closed = true;
static _Bool mouse_panning_drawer = false;
static _Bool dragging_general = false;
static _Bool is_general_released_inside_deploy_area = false;
static _Bool is_general_released_inside_drawer = false;
static _Bool try_once = true;
static _Bool general_placement_valid = false;
static _Bool button_main_menu_clicked = false;
static _Bool button_init_battle_clicked = false;

static unsigned int mouse_left_button_holding_down_counter = 0;

static signed int mouse_x = 0;
static signed int mouse_y = 0;
static float mouse_dragging_origin_x = 0.0f;

static float elapsed = 0.0f;
static float duration = 1.5f;

static SDL_Rect battleplan_grid = {
    0,
    0,
    (signed int)WINDOW_SIZE_X,
    (signed int)WINDOW_SIZE_Y
};

static signed int const window_edge_padding_x = 150;
static signed int const window_edge_padding_y = 100;
static signed int const grid_cell_width_y = (signed int)(WINDOW_SIZE_Y - 300) / GRID_DIMENSION_Y;
static signed int const grid_cell_width_x = grid_cell_width_y + 20;

static SDL_Rect deployment_area = {
    window_edge_padding_x,
    window_edge_padding_y,
    grid_cell_width_x * GRID_DIMENSION_X,
    grid_cell_width_y * GRID_DIMENSION_Y
};

static General *grid[GRID_DIMENSION_X][GRID_DIMENSION_Y];
//static unsigned char grid_general_count = 0;
static DrawerSlot drawer_slot[10]; // size needs to be established by general count in inventory

#define DRAWER_HANDLE_THICKNESS 40

#define GENERAL_SCREEN_WIDTH 100
#define GENERAL_SCREEN_HEIGHT 100


//static signed int drawer_padding_x = 50;
static signed int const drawer_padding_y = 25;
static signed int const padding_between_generals_x = GENERAL_SCREEN_WIDTH + 50;


static signed int pan_offset = 0;
//static signed int scroll_offset = 0;

void battleplan_init(void) {

    load_sprites_into_memory();
    booleans_init();
    variables_init();
    animation_init();

    // this resets the value as it persists between scenes. battleplan needs a future dedicated struct init/reset function
    battleplan.general_in_grid_count = 0;

    signed int drawer_compartment_size = 200;
    drawer = (SDL_Rect){
        0,
        (signed int)WINDOW_SIZE_Y,
        (signed int)WINDOW_SIZE_X,
        drawer_compartment_size
    };

    //signed int drawer_handle_thickness = 40;
    drawer_handle = (SDL_Rect){
        0,
        (signed int)WINDOW_SIZE_Y - DRAWER_HANDLE_THICKNESS,
        (signed int)WINDOW_SIZE_X,
        DRAWER_HANDLE_THICKNESS
    };

    SDL_Renderer *renderer = engine.renderer;

    engine.battleplan = &battleplan;

    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init failed: %s", TTF_GetError());
    }
    
    battleplan.background.r = 130;
    battleplan.background.g = 43;
    battleplan.background.b = 95;
    battleplan.background.a = 255;

    battleplan.button_main_menu.x = 400;
    battleplan.button_main_menu.y = 400;
    battleplan.button_main_menu.w = 100;
    battleplan.button_main_menu.h = 40;
    battleplan.button_main_menu.r = 255;
    battleplan.button_main_menu.g = 255;
    battleplan.button_main_menu.b = 255;
    battleplan.button_main_menu.a = 255;
    battleplan.button_main_menu.text = "Go back to main menu";

    battleplan.button_init_battle.x = 900;
    battleplan.button_init_battle.y = 500;
    battleplan.button_init_battle.w = 100;
    battleplan.button_init_battle.h = 20;
    battleplan.button_init_battle.r = 255;
    battleplan.button_init_battle.g = 255;
    battleplan.button_init_battle.b = 255;
    battleplan.button_init_battle.a = 255;
    battleplan.button_init_battle.text = "Init battle";

    battleplan_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!battleplan_font) {
        SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
        return;
    }

    battleplan_surface = TTF_RenderText_Solid(battleplan_font, "play button", battleplan_white);
    if (!battleplan_surface) {
        SDL_Log("TTF_RenderText_Solid failed: %s", TTF_GetError());
        return;
    }

    battleplan_message = SDL_CreateTextureFromSurface(renderer, battleplan_surface);
    SDL_FreeSurface(battleplan_surface);
    battleplan_surface = NULL;
    TTF_CloseFont(battleplan_font);
    battleplan_font = NULL;

    if (!battleplan_message) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return;
    }

    General *general = inventory_init();

    clear_payload(&drag_payload);

    set_drawer_slot_generals_index(general);

    set_minimal_general_sprite_and_animation_data(general);
    
}

void battleplan_input(SDL_Event *e) {

    if (e->button.x != 0 && e->button.y != 0) {

        mouse_x = e->button.x;
        mouse_y = e->button.y;

    }

    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_LEFT) {

        on_mouse_left_click();

    }

    if (e->type == SDL_MOUSEBUTTONUP &&
        e->button.button == SDL_BUTTON_LEFT) {

        on_left_mouse_release();

    }

}

void battleplan_update(void) {

    if (button_main_menu_clicked) {
        
        scene_switch(MAIN_MENU);
        return;
        
    }

    if (button_init_battle_clicked) {
        
        if (grid_is_empty() == false) {
        
            save_battleplan_placement();

            scene_switch(BATTLEFIELD);
            return;
        
        }

    }

    handle_mouse_left_button();

    if (open_drawer) {

        open_general_drawer();

    }

    if (close_drawer) {

        close_general_drawer();

    }

    void *general = drawer_slot[0].general;
    animation_update(
        general,
        sizeof(General),
        engine.inventory->general_count,
        general_field_table,
        G_ANIM_FIELD,
        G_SPRITE_FIELD
    );

}

void battleplan_render(void) {

    if (battleplan_message == NULL) return;

    SDL_Renderer *renderer = engine.renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect battleplan_background = {0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y};
    SDL_SetRenderDrawColor(
        renderer,
        battleplan.background.r,
        battleplan.background.g,
        battleplan.background.b,
        battleplan.background.a
    );
    SDL_RenderFillRect(renderer, &battleplan_background);

    battleplan_render_deployment_area(renderer);
    battleplan_render_general_drawer(renderer);

    SDL_Rect battleplan_button_main_menu = {
        battleplan.button_main_menu.x,
        battleplan.button_main_menu.y,
        battleplan.button_main_menu.w,
        battleplan.button_main_menu.h 
    };
    SDL_SetRenderDrawColor(
        renderer,
        battleplan.button_main_menu.r,
        battleplan.button_main_menu.g,
        battleplan.button_main_menu.b,
        battleplan.button_main_menu.a
    );
    SDL_RenderFillRect(renderer, &battleplan_button_main_menu);

    SDL_RenderCopy(renderer, battleplan_message, NULL, &battleplan_button_main_menu);

    SDL_Rect battleplan_button_init_battle = {
        battleplan.button_init_battle.x,
        battleplan.button_init_battle.y,
        battleplan.button_init_battle.w,
        battleplan.button_init_battle.h 
    };
    SDL_SetRenderDrawColor(
        renderer,
        battleplan.button_init_battle.r,
        battleplan.button_init_battle.g,
        battleplan.button_init_battle.b,
        battleplan.button_init_battle.a
    );
    SDL_RenderFillRect(renderer, &battleplan_button_init_battle);

    if (drag_payload.general != NULL) {

        render_dragged_general(renderer);

    }

    SDL_RenderCopy(renderer, battleplan_message, NULL, &battleplan_button_init_battle);

    SDL_RenderPresent(renderer);

}

void battleplan_destroy(void) {
    
    if (battleplan_message) {
        SDL_DestroyTexture(battleplan_message);
        battleplan_message = NULL;
    }

    TTF_Quit();

    for (unsigned int i = 0; i < GRID_DIMENSION_X; i++) {
        
        for (unsigned int j = 0; j < GRID_DIMENSION_Y; j++ ) {

            grid[i][j] = NULL;

        }

    }

    //animation_destroy();
    memory_arena_reset();

}

// static functions

static void booleans_init(void) {

    left_mouse_pressed = false;
    holding_mouse = false;
    open_drawer = false;
    close_drawer = false;
    drawer_opened = false;
    drawer_closed = true;
    mouse_panning_drawer = false;
    dragging_general = false;
    is_general_released_inside_deploy_area = false;
    is_general_released_inside_drawer = false;
    try_once = true;
    general_placement_valid = false;
    button_main_menu_clicked = false;

}

static void variables_init(void) {

    mouse_left_button_holding_down_counter = 0;
    mouse_x = 0;
    mouse_y = 0;
    mouse_dragging_origin_x = 0.0f;
    elapsed = 0.0f;

}

static void on_mouse_left_click(void) {
    
    left_mouse_pressed = true;

    button_main_menu_clicked =
        mouse_x >= battleplan.button_main_menu.x &&
        mouse_x <  battleplan.button_main_menu.x +
        battleplan.button_main_menu.w &&
        mouse_y >= battleplan.button_main_menu.y &&
        mouse_y <  battleplan.button_main_menu.y +
        battleplan.button_main_menu.h;
    
    button_init_battle_clicked =
        mouse_x >= battleplan.button_init_battle.x &&
        mouse_x <  battleplan.button_init_battle.x +
        battleplan.button_init_battle.w &&
        mouse_y >= battleplan.button_init_battle.y &&
        mouse_y <  battleplan.button_init_battle.y +
        battleplan.button_init_battle.h;

    if (drawer_closed) {

        open_drawer =
            mouse_x >= drawer_handle.x &&
            mouse_x <  drawer_handle.x + drawer_handle.w &&
            mouse_y >= drawer_handle.y &&
            mouse_y <  drawer_handle.y + drawer_handle.h;

    }

    if (drawer_opened) {

        close_drawer =
            mouse_x >= drawer_handle.x &&
            mouse_x <  drawer_handle.x + drawer_handle.w &&
            mouse_y >= drawer_handle.y &&
            mouse_y <  drawer_handle.y + drawer_handle.h;

    }

}

static void on_left_mouse_release(void) {

    left_mouse_pressed = false;
    holding_mouse = false;
    mouse_panning_drawer = false;
    try_once = true;
    mouse_dragging_origin_x = 0;
    mouse_left_button_holding_down_counter = 0;

    if (dragging_general) {

        dragging_general = false;

        is_general_released_inside_deploy_area =
            mouse_x >= deployment_area.x &&
            mouse_x <  deployment_area.x + deployment_area.w &&
            mouse_y >= deployment_area.y &&
            mouse_y <  deployment_area.y + deployment_area.h;

        is_general_released_inside_drawer =
            mouse_x >= drawer.x &&
            mouse_x < drawer.x + drawer.w &&
            mouse_y >= drawer.y &&
            mouse_y < drawer.y + drawer.h;  

        general_placement_valid =
            is_general_released_inside_deploy_area ||
            is_general_released_inside_drawer;

    }

}

static void handle_mouse_left_button(void) {

    // on click

    if (left_mouse_pressed) {

        if (try_once) {
            
            mouse_dragging_origin_x = mouse_x - pan_offset;
            mouse_panning_drawer =
                mouse_x >= drawer.x &&
                mouse_x <  drawer.x + drawer.w &&
                mouse_y >= drawer.y &&
                mouse_y <  drawer.y + drawer.h;

        }

        // panning through generals
        if (mouse_panning_drawer && !dragging_general) {

            pan_offset = mouse_x - mouse_dragging_origin_x;

        }

        mouse_left_button_holding_down_counter++;
        if (mouse_left_button_holding_down_counter == 10) {

            holding_mouse = true;
            try_once = true;

        }

        if (holding_mouse && try_once) {

            signed int return_value = 0;
            return_value = check_if_dragging_general_out_of_grid();
            if (return_value == -1) {
                check_if_dragging_general_out_of_drawer();
            }
            try_once = false;

        }

    }

    // release

    if (!left_mouse_pressed) {

        if (!dragging_general && general_placement_valid) {

            if (is_general_released_inside_deploy_area) {

                signed int column = floor((mouse_x - window_edge_padding_x) / grid_cell_width_x);
                signed int row = floor((mouse_y - window_edge_padding_y) / grid_cell_width_y);

                if (0 <= column && column < GRID_DIMENSION_X && 0 <= row && row < GRID_DIMENSION_Y) {

                    if (grid[column][row] == NULL) {

                        General **grid_cell = &grid[column][row];
                        General **general_to_place = &drag_payload.general;
                        place_general_on_grid(grid_cell, general_to_place);
                        battleplan.general_in_grid_count++;

                    } else {

                        General **general_in_grid = &grid[column][row];
                        swap_generals(general_in_grid, &drag_payload);

                    }

                } else {
                    
                    drag_payload.general->render = true;

                }

                //clear_payload(&drag_payload);


            } else if (is_general_released_inside_drawer && drag_payload.origin != DRAWER) {
                signed int x = drag_payload.grid_cell_x;
                signed int y = drag_payload.grid_cell_y;
                if (x != -1 && y != -1) {

                    grid[x][y] = NULL;
                    General **general = &drag_payload.general;
                    insert_general_into_drawer(general);
                    battleplan.general_in_grid_count--;

                } else {

                    printf("x = %d, y = %d\n", x, y);
                    fprintf(stderr, "x or y is -1 even though payload origin is from grid\n");
                    exit(EXIT_FAILURE);
                    //drag_payload.general->render = true;
                }

                //clear_payload(&drag_payload);

            } else if (is_general_released_inside_drawer && drag_payload.origin == DRAWER) {
                
                drag_payload.general->render = true;
                //clear_payload(&drag_payload);

            } else {

                fprintf(stderr, "logic failure. this branch should not possibly be entered\n");
                exit(EXIT_FAILURE);
            
            }

            general_placement_valid = false;
            is_general_released_inside_deploy_area = false;
            is_general_released_inside_drawer = false;
            clear_payload(&drag_payload);

        } else if (!dragging_general && !general_placement_valid) {

            if (drag_payload.origin == GRID) {

                is_general_released_inside_deploy_area = false;
                is_general_released_inside_drawer = false;
                clear_payload(&drag_payload);

            }

            if (drag_payload.origin == DRAWER) {
                
                is_general_released_inside_deploy_area = false;
                is_general_released_inside_drawer = false;
                drag_payload.general->render = true;
                clear_payload(&drag_payload);

            }

        }

    }

}

static signed int check_if_dragging_general_out_of_grid(void) {

    signed int column = floor((mouse_x - window_edge_padding_x) / grid_cell_width_x);
    signed int row = floor((mouse_y - window_edge_padding_y) / grid_cell_width_y);

    if (0 <= column && column < GRID_DIMENSION_X && 0 <= row && row < GRID_DIMENSION_Y) {

        if (grid[column][row] != NULL) {

            dragging_general = true;
            drag_payload.general = grid[column][row];
            drag_payload.general->render = false;
            drag_payload.grid_cell_x = column;
            drag_payload.grid_cell_y = row;
            drag_payload.origin = GRID;
            return GRID;

        } else {

            try_once = false;

        }

    }

    return -1;

}

static void check_if_dragging_general_out_of_drawer(void) {

    signed int j = 0;
    for (signed int i = 0; i < engine.inventory->general_count; i++){
        
        if (drawer_slot[i].general->render == false) {
            j++;
            continue;
        }

        dragging_general =
            mouse_x >= (i - j) * padding_between_generals_x + padding_between_generals_x - pan_offset &&
            mouse_x <  GENERAL_SCREEN_WIDTH + (i - j) * padding_between_generals_x + padding_between_generals_x - pan_offset &&
            mouse_y >= drawer_padding_y + drawer.y &&
            mouse_y <  drawer_padding_y + drawer.y + drawer.h + GENERAL_SCREEN_HEIGHT;

        if (dragging_general) {

            drag_payload.origin = DRAWER;
            drag_payload.general = drawer_slot[i].general;
            drag_payload.general->render = false;
            drag_payload.grid_cell_x = -1;
            drag_payload.grid_cell_y = -1;
            break;
        
        }
    
    }

}

static void battleplan_render_deployment_area(SDL_Renderer *renderer) {

    SDL_SetRenderDrawColor(
        renderer,
        128,
        128,
        128,
        255
    );
    SDL_RenderFillRect(renderer, &battleplan_grid);

    SDL_SetRenderDrawColor(
        renderer,
        123,
        123,
        123,
        255
    );
    SDL_RenderFillRect(renderer, &deployment_area);
    
    signed int grid_fishnet_line_width = 6;
    signed int x = deployment_area.x - grid_fishnet_line_width / 2/* + deployment_area.w / GRID_DIMENSION_X*/;
    signed int offset_x = deployment_area.w / GRID_DIMENSION_X;
    for (signed int i = 0; i < GRID_DIMENSION_X + 1; i++) {
        
        SDL_Rect grid_line_vertical = {
            x + i * offset_x,
            deployment_area.y - 25,
            grid_fishnet_line_width,
            deployment_area.h + 50
        };
        SDL_SetRenderDrawColor(
            renderer,
            0,
            0,
            0,
            255
        );
        SDL_RenderFillRect(renderer, &grid_line_vertical);

    }

    signed int y = deployment_area.y - grid_fishnet_line_width / 2/* + deployment_area.h / GRID_DIMENSION_Y*/;
    signed int offset_y = deployment_area.h / GRID_DIMENSION_Y;
    for (signed int i = 0; i < GRID_DIMENSION_Y + 1; i++) {
        
        SDL_Rect grid_line_horizontal = {
            deployment_area.x - 25,
            y + i * offset_y,
            deployment_area.w + 50,
            grid_fishnet_line_width
        };
        SDL_SetRenderDrawColor(
            renderer,
            0,
            0,
            0,
            255
        );
        SDL_RenderFillRect(renderer, &grid_line_horizontal);

    }


    // second area
    SDL_Rect copy_deployment_area = {
        deployment_area.x,
        deployment_area.y,
        deployment_area.w,
        deployment_area.h
    };

    copy_deployment_area.x = (signed int)WINDOW_SIZE_X - copy_deployment_area.x - GRID_DIMENSION_X * grid_cell_width_x;
    SDL_SetRenderDrawColor(
        renderer,
        123,
        123,
        123,
        255
    );
    SDL_RenderFillRect(renderer, &copy_deployment_area);
    
    x = copy_deployment_area.x - grid_fishnet_line_width / 2/* + deployment_area.w / GRID_DIMENSION_X*/;
    for (signed int i = 0; i < GRID_DIMENSION_X + 1; i++) {
        
        SDL_Rect grid_line_vertical = {
            x + i * offset_x,
            copy_deployment_area.y - 25,
            grid_fishnet_line_width,
            copy_deployment_area.h + 50
        };
        SDL_SetRenderDrawColor(
            renderer,
            0,
            0,
            0,
            255
        );
        SDL_RenderFillRect(renderer, &grid_line_vertical);

    }

    for (signed int i = 0; i < GRID_DIMENSION_Y + 1; i++) {
        
        SDL_Rect grid_line_horizontal = {
            copy_deployment_area.x - 25,
            y + i * offset_y,
            copy_deployment_area.w + 50,
            grid_fishnet_line_width
        };
        SDL_SetRenderDrawColor(
            renderer,
            0,
            0,
            0,
            255
        );
        SDL_RenderFillRect(renderer, &grid_line_horizontal);

    }

    render_grid_generals(renderer);

}

static void battleplan_render_general_drawer(SDL_Renderer *renderer) {

    SDL_SetRenderDrawColor(
        renderer,
        54,
        69,
        79,
        255
    );
    SDL_RenderFillRect(renderer, &drawer_handle);

    if (!drawer_closed) {

        SDL_SetRenderDrawColor(
            renderer,
            18,
            142,
            58,
            255
        );
        SDL_RenderFillRect(renderer, &drawer);

        render_drawer_generals(renderer);

    }

}

static void render_drawer_generals(SDL_Renderer *renderer) {

    Sprite (*sprites)[ANIMATION_COUNT] = engine.sprite_pack->sprite;
    General *general = drawer_slot->general;
    unsigned int mult = 0;
    for (unsigned int i = 0; i < engine.inventory->general_count; i++) {

        if (general[i].render) {

            Sprite *sprite = &sprites[general[i].sprite.type][general[i].anim.animation];
            
            signed int png_width = sprite->width;
            signed int png_height = sprite->height;
            signed int sprite_frame_width = png_width / sprite->frames_count;
            signed int sprite_frame_height = png_height;

            SDL_Rect sprite_slice = {
                (sprite_frame_width * general[i].anim.current_frame),
                0,
                sprite_frame_width,
                sprite_frame_height
            };

            SDL_Rect sprite_position = {
                (signed int) mult * padding_between_generals_x + padding_between_generals_x - pan_offset,
                (signed int) drawer_padding_y + drawer.y,
                (signed int) GENERAL_SCREEN_WIDTH,
                (signed int) GENERAL_SCREEN_HEIGHT
            };

            //camera_world_to_screen(&sprite_position);

            const SDL_Rect *rect1 = &sprite_slice;
            const SDL_Rect *rect2 = &sprite_position;

            SDL_Texture *texture = sprite->texture;
            SDL_RenderCopy(renderer, texture, rect1, rect2);
        
            mult++;

        }

    }

}

static void render_dragged_general(SDL_Renderer *renderer) {

    General *general = drag_payload.general;

    Sprite *sprite = &engine.sprite_pack->sprite[general->sprite.type][general->anim.animation];
            
    signed int png_width = sprite->width;
    signed int png_height = sprite->height;
    signed int sprite_frame_width = png_width / sprite->frames_count;
    signed int sprite_frame_height = png_height;

    SDL_Rect sprite_slice = {
        (sprite_frame_width * general->anim.current_frame),
        0,
        sprite_frame_width,
        sprite_frame_height
    };

    SDL_Rect sprite_position = {
        (signed int) mouse_x,
        (signed int) mouse_y,
        (signed int) GENERAL_SCREEN_WIDTH,
        (signed int) GENERAL_SCREEN_HEIGHT
    };

    //camera_world_to_screen(&sprite_position);

    const SDL_Rect *rect1 = &sprite_slice;
    const SDL_Rect *rect2 = &sprite_position;

    SDL_Texture *texture = sprite->texture;
    SDL_RenderCopy(renderer, texture, rect1, rect2);

}

static void render_grid_generals(SDL_Renderer *renderer) {

    Sprite (*sprites)[ANIMATION_COUNT] = engine.sprite_pack->sprite;

    signed int x = deployment_area.x;
    signed int offset_x = deployment_area.w / GRID_DIMENSION_X;
    signed int y = deployment_area.y;
    signed int offset_y = deployment_area.h / GRID_DIMENSION_Y;
        
    for (signed int i = 0; i < GRID_DIMENSION_X; i++) {

        for (signed int j = 0; j < GRID_DIMENSION_Y; j++) {

            if (grid[i][j] != NULL) {

                General *general = grid[i][j];

                Sprite *sprite = &sprites[general->sprite.type][general->anim.animation];
                        
                signed int png_width = sprite->width;
                signed int png_height = sprite->height;
                signed int sprite_frame_width = png_width / sprite->frames_count;
                signed int sprite_frame_height = png_height;

                SDL_Rect sprite_slice = {
                    (sprite_frame_width * general->anim.current_frame),
                    0,
                    sprite_frame_width,
                    sprite_frame_height
                };

                SDL_Rect sprite_position = {
                    (signed int) x + i * offset_x,
                    (signed int) y + j * offset_y,
                    (signed int) grid_cell_width_x,
                    (signed int) grid_cell_width_y
                };

                //camera_world_to_screen(&sprite_position);

                const SDL_Rect *rect1 = &sprite_slice;
                const SDL_Rect *rect2 = &sprite_position;

                SDL_Texture *texture = sprite->texture;

                signed int g_x = drag_payload.grid_cell_x;
                signed int g_y = drag_payload.grid_cell_y;
                if (i == g_x && j == g_y) {

                    SDL_SetTextureAlphaMod(texture, 128);
                    SDL_RenderCopy(renderer, texture, rect1, rect2);
                    SDL_SetTextureAlphaMod(texture, 255);

                } else {

                    SDL_RenderCopy(renderer, texture, rect1, rect2);

                }

            }

        }

    }

}

static void open_general_drawer(void) {

    float a = WINDOW_SIZE_Y - 200;
    float b = WINDOW_SIZE_Y;

    float t = elapsed / duration;
    elapsed += engine.game->delta;
    float y = ease_out_elastic(t);

    signed int result = (signed int)(b + (a - b) * y);
    
    drawer_handle.y = result - drawer_handle.h;
    drawer.y = result;

}

static void close_general_drawer(void) {

    float a = WINDOW_SIZE_Y - 200;
    float b = WINDOW_SIZE_Y;
    float t = elapsed / duration;
    elapsed += engine.game->delta;
    float y = ease_out_elastic(t);

    signed int result = (signed int)(a + (b - a) * y);
    
    drawer_handle.y = result - drawer_handle.h;
    drawer.y = result;

}

static float ease_out_elastic(float x) {

    if (x == 0.0f) {

        drawer_closed = false;
        drawer_opened = false;
        return 0.0f;

    }

    if (x >= 1.0f) {

        if (open_drawer) {

            drawer_opened = true;
            open_drawer = false;

        }

        if (close_drawer) {

            drawer_closed = true;
            close_drawer = false;

        }

        elapsed = 0.0f;
        return 1.0f;

    }

    return powf(2.0f, (-10.0f * x)) * sinf((x * 10.0f - 0.75f) * 2.094395f) + 1.0f; //2.094395f is 2 * pi / 3 approximately

}

/*static float ease_in_elastic(float x) {

    if (x == 0.0f) {
        printf("y = %f\n", (double)x);
        drawer_opened = false;
        return 0.0f;
    }
    if (x >= 1.0f) {
        printf("yyyy = %f\n", (double)x);
        close_drawer = false;
        drawer_closed = true;
        elapsed = 0.0f;
        return 1.0f;
    }
    return -powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * 2.094395f); //2.094395f is 2 * pi / 3 approximately

}*/

static void set_drawer_slot_generals_index(General *general) {

    unsigned int index = 0;
    for (enum Rarity i = 0; i < RARITY_TYPES_COUNT; i++) {

        for (unsigned int j = 0; j < engine.inventory->general_count; j++) {

            if (general[j].rarity == i) {

                drawer_slot[index++].general = &general[j];
            
            }
        
        }

    }

}

static void set_minimal_general_sprite_and_animation_data(General *general) {

    for (unsigned int i = 0; i < engine.inventory->general_count; i++) {

        general[i].render = true;
        general[i].anim.animation = IDLE;
        general[i].anim.frames_count = engine.sprite_pack->sprite[general[i].sprite.type][IDLE].frames_count;
        general[i].anim.current_frame = (unsigned char)(rand() % general[i].anim.frames_count);

    }

}

static void place_general_on_grid(General **grid_cell, General **general) { // talvez param 2 poderia ser so Gen *, nao **

    *grid_cell = *general;
    
    if (drag_payload.origin == GRID) {
        
        signed int x = drag_payload.grid_cell_x;
        signed int y = drag_payload.grid_cell_y;
        grid[x][y] = NULL;
        
    }

}

static void swap_generals(General **general_in_grid, DragPayload *payload) {

    General **general_to_swap = &payload->general;

    if (payload->origin == DRAWER) {

        insert_general_into_drawer(general_in_grid);
        place_general_on_grid(general_in_grid, general_to_swap);
    
    }

    if (payload->origin == GRID) {

        General *general = *general_in_grid;
        place_general_on_grid(general_in_grid, general_to_swap);

        signed int x = payload->grid_cell_x;
        signed int y = payload->grid_cell_y;
        grid[x][y] = general;

    }

}

static void insert_general_into_drawer(General **general_to_insert) {

    (*general_to_insert)->render = true;
    *general_to_insert = NULL;

}

static void clear_payload(DragPayload *payload) {

    payload->general = NULL;
    payload->grid_cell_x = -1;
    payload->grid_cell_y = -1;
    payload->origin = NO_ORIGIN;

}

static void save_battleplan_placement(void) {

    if (battleplan.grid_payload != NULL) {

        printf("battleplan_general_placement was not freed prior to this assignment\n");
        free(battleplan.grid_payload);
    
    }

    GridPlacementPayload *buffer = calloc(1, sizeof(GridPlacementPayload) + sizeof(OccupiedCell) * battleplan.general_in_grid_count);
    buffer->grid.dimension.x = GRID_DIMENSION_X;
    buffer->grid.dimension.y = GRID_DIMENSION_Y;
    buffer->occupied_cell_count = battleplan.general_in_grid_count;

    unsigned int idx = 0;

    for (unsigned int i = 0; i < GRID_DIMENSION_X; i++) {

        for (unsigned int j = 0; j < GRID_DIMENSION_Y; j++) {

            if (grid[i][j] != NULL) {
                
                buffer->occupied_cell[idx].x = i;
                buffer->occupied_cell[idx].y = j;
                memcpy(&buffer->occupied_cell[idx].general, grid[i][j], sizeof(General));
                idx++;
                if (idx == battleplan.general_in_grid_count) {
                    battleplan.grid_payload = buffer;
                    return;
                }
            }

        }

    }

    battleplan.grid_payload = buffer;

}

static _Bool grid_is_empty(void) {

    for (signed int i = 0; i < GRID_DIMENSION_X; i++) {

        for (signed int j = 0; j < GRID_DIMENSION_Y; j++) {

            if (grid[i][j] != NULL) {
            
                return false;
            
            }
        
        }
        
    }

    return true;

}