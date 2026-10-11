#ifndef FATIGUE_H
#define FATIGUE_H

//#include "battalion_types.h"
#include "fatigue_types.h"

//void fatigue_update(Battalion *battalion, float dt);
void fatigue_update(Fatigue *fatigue, _Bool is_active, _Bool is_fighting, float dt);

#endif