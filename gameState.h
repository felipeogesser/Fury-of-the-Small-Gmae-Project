#ifndef GAMESTATE
#define GAMESTATE

#define INVSQRT2 0.70710678f

enum SceneState {
    MAIN_MENU,
    //VILLAGE,
    //BATTLEPLAN,
    BATTLEFIELD
};

typedef struct GameState {
    enum SceneState scene_state;
    float LX, LY;
    float KX, KY;
    float offSetX, offSetY;
    float vxdt, vydt;
    float dirUp, dirDown, dirLeft, dirRight;
    float vx, vy, va, vb;
    float delta;
    unsigned int low_LOD_quadrant_size;
    unsigned int medium_LOD_quadrant_size;
    unsigned int high_LOD_quadrant_size;
    unsigned int low_LOD_quadrant_count;
    unsigned int medium_LOD_quadrant_count;
    unsigned int high_LOD_quadrant_count;
    signed int amountX, amountY;
    float decay_rate;
    unsigned int entities_created_count, objects_created_count;
} GameState;

GameState *pass_pointer(void);

#endif