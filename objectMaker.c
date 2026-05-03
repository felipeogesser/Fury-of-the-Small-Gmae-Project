#include "objectMaker.h"

int object_id[MAX_OBJECTS];
void make_objects(void) {
    object_id[0] = create_object("wallV", 150.0f, 100.0f, 30.0f, 400.0f, 255, 0, 0, 255, true);
    object_id[1] = create_object("wallH", 200.0f, 150.0f, 300.0f, 40.0f, 255, 0, 0, 255, true);
    object_id[2] = create_object("wallI", 300.0f, 200.0f, 300.0f, 40.0f, 255, 0, 0, 255, false);
}