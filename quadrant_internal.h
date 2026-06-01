#ifndef QUADRANT_INTERNAL_H
#define QUADRANT_INTERNAL_H

typedef struct bigQuadrant {
    unsigned short id;
    unsigned short local_entities_count;
    unsigned char smell;
    unsigned int entitiesInsideQuadrantById[20];
} bigQuadrant;

typedef struct mediumQuadrant {
    unsigned char smell;
} mediumQuadrant;

typedef struct smallQuadrant {
    unsigned char smell;
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