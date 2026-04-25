#include "objects.h"

static Object objects[MAX_OBJECTS];
static int object_count = 0;

Object *object = NULL;


int create_object(const char *name, float pointX, float pointY, float dimensionX,
    float dimensionY, int R_Color, int G_Color, int B_Color, int Alpha,
    bool collision, int quadrant) {
    if (object_count >= MAX_OBJECTS) return -1; 
    int objectIndex = object_count;
    objects[objectIndex].id = objectIndex + 1;
    snprintf(objects[objectIndex].name, sizeof objects[objectIndex].name, "%s", name);
    objects[objectIndex].pointX = pointX;
    objects[objectIndex].pointY = pointY;
    objects[objectIndex].dimensionX = dimensionX;
    objects[objectIndex].dimensionY = dimensionY;
    objects[objectIndex].R_Color = R_Color;
    objects[objectIndex].G_Color = G_Color;
    objects[objectIndex].B_Color = B_Color;
    objects[objectIndex].Alpha = Alpha;
    objects[objectIndex].collision = collision;
    objects[objectIndex].quadrant = quadrant;
    object_count++;
    return objects[objectIndex].id;
}

Object *get_object(int objectId){
    if (objectId <= 0 || objectId > object_count) return NULL;
    return &objects[objectId - 1];
}