#include "objects.h"
#include <stdio.h>

static Object objects[MAX_OBJECTS];
static int object_count = 0;

int create_object(const char *name, float pointX, float pointY, float dimensionX, float dimensionY, bool collision, int quadrant) {
    if (object_count >= MAX_OBJECTS) return -1; 
    int id = object_count;
    objects[id].id = id;
    snprintf(objects[id].name, sizeof objects[id].name, "%s", name);
    objects[id].pointX = pointX;
    objects[id].pointY = pointY;
    objects[id].dimensionX = dimensionX;
    objects[id].dimensionY = dimensionY;
    objects[id].collision = collision;
    objects[id].quadrant = quadrant;
    object_count++;
    return id;
}

Object *get_object(int id){
    if (id < 0 || id >= object_count) return NULL;
    return &objects[id];
}