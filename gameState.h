#ifndef GAMESTATE
#define GAMESTATE

#define INVSQRT2 0.70710678f

typedef struct GameState {
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
    signed int amountX, amountY;
    float decay_rate;
    unsigned int entities_created_count, objects_created_count;
} GameState;

extern GameState *game;

#endif