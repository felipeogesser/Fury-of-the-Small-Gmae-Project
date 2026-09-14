#ifndef EOC
#define EOC

#include "unit.h"
#include "quadrant.h"
#include "gameState.h"
#include "objects.h"

#define CORNER1   (1U << 0)
#define CORNER2   (1U << 1)
#define CORNER3   (1U << 2)
#define CORNER4   (1U << 3)
#define FLIPVECX  (1U << 4)
#define FLIPVECY  (1U << 5)
#define FLIPVECX2 (1U << 6)
#define FLIPVECY2 (1U << 7)

void unit_object_collision(Unit *unit, Quadrant *quadrant, float (*hitBoxObject)[4][2], GameState *game, int x, int y);

#endif