#ifndef GAMESTATE
#define GAMESTATE

#define INVSQRT2 0.70710678f

typedef struct GameState {
    float LX, LY;
    float KX, KY;
    float ZX, ZY;
    float vxdt, vydt;
    float dirUp, dirDown, dirLeft, dirRight;
    float vx, vy, va, vb;
} GameState;

// inline int modify_gameState(GameState *game);

#endif