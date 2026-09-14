#ifndef QUADRANT_INTERNAL_H
#define QUADRANT_INTERNAL_H

typedef struct bigQuadrant {
    unsigned short id;
    unsigned short local_unit_count;
    unsigned char smell;
    unsigned int next_array_slot;
    unsigned int unit_inside_quad[1000];
} bigQuadrant;

typedef struct mediumQuadrant {
    unsigned short id;
    unsigned short local_unit_count;
    unsigned char smell;
    unsigned int next_array_slot;
    unsigned int unit_inside_quad[1000];
} mediumQuadrant;

typedef struct smallQuadrant {
    unsigned short id;
    unsigned short local_unit_count;
    unsigned char smell;
    unsigned int next_array_slot;
    unsigned int unit_inside_quad[1000];
} smallQuadrant;

/*typedef struct unitInsideQuadrant {
    int unitInsideQuadrantById[10];
    int unitInQuadrantCount;
} unitInsideQuadrant;

typedef struct Quadrant {
    int id;
    struct unitInsideQuadrant innerEntsInQuad;
    int objectsInQuadrant[MAX_OBJECTS];
} Quadrant;*/

#endif