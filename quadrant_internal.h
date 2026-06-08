#ifndef QUADRANT_INTERNAL_H
#define QUADRANT_INTERNAL_H

typedef struct bigQuadrant {
    unsigned short id;
    unsigned short local_entities_count;
    unsigned char smell;
    unsigned int next_array_slot;
    unsigned int entities_inside_quad[1000];
} bigQuadrant;

typedef struct mediumQuadrant {
    unsigned short id;
    unsigned short local_entities_count;
    unsigned char smell;
    unsigned int next_array_slot;
    unsigned int entities_inside_quad[1000];
} mediumQuadrant;

typedef struct smallQuadrant {
    unsigned short id;
    unsigned short local_entities_count;
    unsigned char smell;
    unsigned int next_array_slot;
    unsigned int entities_inside_quad[1000];
} smallQuadrant;

/*typedef struct EntitiesInsideQuadrant {
    int entitiesInsideQuadrantById[10];
    int entitiesInQuadrantCount;
} EntitiesInsideQuadrant;

typedef struct Quadrant {
    int id;
    struct EntitiesInsideQuadrant innerEntsInQuad;
    int objectsInQuadrant[MAX_OBJECTS];
} Quadrant;*/

#endif