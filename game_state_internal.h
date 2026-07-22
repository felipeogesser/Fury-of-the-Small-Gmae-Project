#ifndef GAME_STATE_INTERNAL_H
#define GAME_STATE_INTERNAL_H

#include "scenes.h"

#define INVSQRT2 0.70710678f

typedef struct GameState {
    enum SceneState scene_state;
    float LX, LY;
    float KX, KY;
    //float offSetX, offSetY;
    float delta;
    float decay_rate;
    unsigned int entities_created_count, objects_created_count;
    unsigned short low_LOD_quadrant_count;
    unsigned short low_LOD_quadrant_size;
    unsigned short medium_LOD_quadrant_count;
    unsigned short medium_LOD_quadrant_size;
    unsigned short high_LOD_quadrant_count;
    unsigned short high_LOD_quadrant_size;
    unsigned short amountX, amountY;
} GameState;

#endif