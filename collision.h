#ifndef COLLISION_H
#define COLLISION_H

void resolve_collisions(float *unit_x, float *unit_y, const _Bool *unit_alive,
                        unsigned int unit_count, unsigned int unit_alive_count,
                        float min_dist, unsigned int passes);
void resolve_collisions_between(float *a_x, float *a_y, const _Bool *a_alive,
                                unsigned int a_count, unsigned int a_alive_count,
                                float *b_x, float *b_y, const _Bool *b_alive,
                                unsigned int b_count, unsigned int b_alive_count,
                                float min_dist, unsigned int passes);

#endif