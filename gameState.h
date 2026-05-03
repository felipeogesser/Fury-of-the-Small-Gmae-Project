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
    unsigned int quadrantSize;
    signed int amountX, amountY;
    float decay_rate;
} GameState;

extern GameState *game;

#endif