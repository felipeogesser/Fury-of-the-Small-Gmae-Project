#include "battleplan.h"
#include "battleplan_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "scene_handler.h"
#include "scenes.h"
#include "window_settings.h"
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

// private prototypes

static void battleplan_render_deployment_area(SDL_Renderer *renderer);
static void battleplan_render_general_drawer(SDL_Renderer *renderer);
static void open_general_drawer(void);
static float ease_out_elastic(float x);

static Battleplan battleplan = {0};

static TTF_Font *battleplan_font = NULL;

static SDL_Color battleplan_white = {255, 255, 255, 255};

static SDL_Surface *battleplan_surface = NULL; 

static SDL_Texture *battleplan_message = NULL;

static SDL_Rect drawer_handle = {0};
static SDL_Rect drawer = {0};

static _Bool drawer_opened = 0;

static float elapsed = 0.0f;
static float duration = 1.5f;
static float a = WINDOW_SIZE_Y;
static float b = WINDOW_SIZE_Y - 200.0f;
static float t = 0.0f;

void battleplan_init(void) {

    signed int drawer_compartment_size = 200;
    drawer = (SDL_Rect){
        0,
        (signed int)WINDOW_SIZE_Y,
        (signed int)WINDOW_SIZE_X,
        drawer_compartment_size
    };

    signed int drawer_handle_thickness = 40;
    drawer_handle = (SDL_Rect){
        0,
        (signed int)WINDOW_SIZE_Y - drawer_handle_thickness,
        (signed int)WINDOW_SIZE_X,
        drawer_handle_thickness
    };

    SDL_Renderer *renderer = engine.renderer;

    engine.battleplan = &battleplan;

    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init failed: %s", TTF_GetError());
    }
    
    battleplan.background_R_color = 130;
    battleplan.background_G_color = 43;
    battleplan.background_B_color = 95;
    battleplan.background_Alpha = 255;

    battleplan.button_main_menu_position_x = 400;
    battleplan.button_main_menu_position_y = 400;
    battleplan.button_main_menu_width_x = 100;
    battleplan.button_main_menu_width_y = 40;
    battleplan.button_main_menu_R_color = 255;
    battleplan.button_main_menu_G_color = 255;
    battleplan.button_main_menu_B_color = 255;
    battleplan.button_main_menu_Alpha = 255;
    battleplan.button_main_menu_text = "Go back to main menu";

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

}

void battleplan_input(SDL_Event *e) {

    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_LEFT) {

        int mouse_x = e->button.x;
        int mouse_y = e->button.y;

        _Bool button_clicked =
            mouse_x >= battleplan.button_main_menu_position_x &&
            mouse_x <  battleplan.button_main_menu_position_x +
            battleplan.button_main_menu_width_x &&
            mouse_y >= battleplan.button_main_menu_position_y &&
            mouse_y <  battleplan.button_main_menu_position_y +
            battleplan.button_main_menu_width_y;

        if (button_clicked) {
            
            scene_switch(MAIN_MENU);
    
        }

        drawer_opened =
            mouse_x >= drawer_handle.x &&
            mouse_x <  drawer_handle.x + drawer_handle.w &&
            mouse_y >= drawer_handle.y &&
            mouse_y <  drawer_handle.y + drawer_handle.h;

    }

}

void battleplan_update(void) {

}

void battleplan_render(void) {

    if (battleplan_message == NULL) return;

    SDL_Renderer *renderer = engine.renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect battleplan_background = {0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y};
    SDL_SetRenderDrawColor(
        renderer,
        battleplan.background_R_color,
        battleplan.background_G_color,
        battleplan.background_B_color,
        battleplan.background_Alpha
    );
    SDL_RenderFillRect(renderer, &battleplan_background);

    battleplan_render_deployment_area(renderer);
    battleplan_render_general_drawer(renderer);

    if (drawer_opened) {

        open_general_drawer();

    }

    SDL_Rect battleplan_button = {
        battleplan.button_main_menu_position_x,
        battleplan.button_main_menu_position_y,
        battleplan.button_main_menu_width_x,
        battleplan.button_main_menu_width_y 
    };
    SDL_SetRenderDrawColor(
        renderer,
        battleplan.button_main_menu_R_color,
        battleplan.button_main_menu_G_color,
        battleplan.button_main_menu_B_color,
        battleplan.button_main_menu_Alpha
    );
    SDL_RenderFillRect(renderer, &battleplan_button);

    SDL_RenderCopy(renderer, battleplan_message, NULL, &battleplan_button);

    SDL_RenderPresent(renderer);

}

void battleplan_destroy(void) {
    
    if (battleplan_message) {
        SDL_DestroyTexture(battleplan_message);
        battleplan_message = NULL;
    }

    TTF_Quit();
}



static void battleplan_render_deployment_area(SDL_Renderer *renderer) {

    SDL_Rect battleplan_grid = {
        0,
        0,
        (signed int)WINDOW_SIZE_X,
        (signed int)WINDOW_SIZE_Y
    };
    SDL_SetRenderDrawColor(
        renderer,
        128,
        128,
        128,
        255
    );
    SDL_RenderFillRect(renderer, &battleplan_grid);


    signed int window_edge_padding_x = 150;
    signed int window_edge_padding_y = 100;
    signed int grid_dimension_x = 5;
    signed int grid_dimension_y = 6;
    signed int grid_cell_dimension_y = (signed int)(WINDOW_SIZE_Y - 300) / grid_dimension_y;
    signed int grid_cell_dimension_x = grid_cell_dimension_y + 20;
    SDL_Rect deployment_area = {
        window_edge_padding_x,
        window_edge_padding_y,
        grid_cell_dimension_x * grid_dimension_x,
        grid_cell_dimension_y * grid_dimension_y
    };
    SDL_SetRenderDrawColor(
        renderer,
        123,
        123,
        123,
        255
    );
    SDL_RenderFillRect(renderer, &deployment_area);
    
    signed int grid_fishnet_line_width = 6;
    signed int x = deployment_area.x - grid_fishnet_line_width / 2/* + deployment_area.w / grid_dimension_x*/;
    signed int offset_x = deployment_area.w / grid_dimension_x;
    for (signed int i = 0; i < grid_dimension_x + 1; i++) {
        
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

    signed int y = deployment_area.y - grid_fishnet_line_width / 2/* + deployment_area.h / grid_dimension_y*/;
    signed int offset_y = deployment_area.h / grid_dimension_y;
    for (signed int i = 0; i < grid_dimension_y + 1; i++) {
        
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

    deployment_area.x = (signed int)WINDOW_SIZE_X - deployment_area.x - grid_dimension_x * grid_cell_dimension_x;
    SDL_SetRenderDrawColor(
        renderer,
        123,
        123,
        123,
        255
    );
    SDL_RenderFillRect(renderer, &deployment_area);
    
    x = deployment_area.x - grid_fishnet_line_width / 2/* + deployment_area.w / grid_dimension_x*/;
    for (signed int i = 0; i < grid_dimension_x + 1; i++) {
        
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

    for (signed int i = 0; i < grid_dimension_y + 1; i++) {
        
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

    SDL_SetRenderDrawColor(
        renderer,
        128,
        128,
        128,
        255
    );
    SDL_RenderFillRect(renderer, &drawer);

}

static void open_general_drawer(void) {

    elapsed += engine.game->delta;
    t = elapsed / duration;
    float y = ease_out_elastic(t);
    drawer.y = (signed int)(a + (b - a) * y) + drawer_handle.h;
    drawer_handle.y = (signed int)(a + (b - a) * y);

}

static float ease_out_elastic(float x) {

    if (x == 0.0f) return 0.0f;
    if (x >= 1.0f) {
        return 1.0f;
        drawer_opened = 0;
    }
    return powf(2.0f, (-10.0f * x)) * sinf((x * 10.0f - 0.75f) * 2.094395f) + 1.0f; //2.094395f is 2 * pi / 3 approximately

}
