#ifndef CAMERA_INTERNAL_H
#define CAMERA_INTERNAL_H

#include "camera_types.h"

typedef struct Camera {
    float zoom, zoom_min, zoom_max;
    float position_x, position_y;
//    unsigned int scale;
} Camera;

#endif