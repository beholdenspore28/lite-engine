#ifndef L_PHYSICS_H
#define L_PHYSICS_H

#include "blib/blib_math3d.h"
#include "lgl.h"

typedef struct {
  vector3 *position_old;
  vector3 *acceleration;
  unsigned int *is_pinned;
  float bounciness;
  float friction;
} l_verlet_body;

l_verlet_body l_verlet_body_alloc(l_object object);
void l_verlet_body_free(l_verlet_body verlet);

void l_verlet_body_update(l_object object, l_verlet_body verlet);

static inline void l_verlet_body_accelerate(l_verlet_body verlet,
                                            unsigned int index,
                                            vector3 force) {
  verlet.acceleration[index] = vector3_add(verlet.acceleration[index], force);
}

void l_verlet_body_confine(l_object object, l_verlet_body verlet,
                           vector3 bounds);

void l_verlet_body_constrain_distance(l_object object, l_verlet_body verlet,
                                      unsigned int point_a,
                                      unsigned int point_b,
                                      float distance_constraint);
#endif // L_PHYSICS_H
