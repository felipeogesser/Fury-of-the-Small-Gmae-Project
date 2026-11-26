#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "projectile_types.h"
#include "table_types.h"

void projectile_reset(void);
void projectile_launch(const ProjectileLaunch *launch, float dt);
void projectile_update(Table *player_tables, Table *enemy_tables, unsigned int battalion_count, unsigned int battalion_size, float dt);
void projectile_render(void);

#endif
