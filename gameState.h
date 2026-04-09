#ifndef GAMESTATE
#define GAMESTATE

typedef struct GameState {
    float LX, LY;
    float KX, KY;
    float ZX, ZY;
    float vxdt, vydt;
} GameState;

// inline int modify_gameState(GameState *game);

#endif