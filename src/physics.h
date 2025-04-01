#ifndef PHYSICS_H
#define PHYSICS_H

#include "blib/blib_math3d.h"
#include "lgl.h"

typedef struct {
  vector3_t *position_old;
  unsigned int *is_pinned;
  float bounce;
  float gravity;
  float friction;
  unsigned int count;
} l_verlet_t;

l_verlet_t l_verlet_alloc(lgl_batch_t batch);
void l_verlet_free(l_verlet_t verlet);

void l_verlet_update(lgl_batch_t batch, l_verlet_t points);

void l_verlet_confine(l_verlet_t verlet, lgl_batch_t batch, vector3_t bounds);

void l_verlet_constrain_distance(lgl_batch_t batch, l_verlet_t verlet,
                                 unsigned int point_a, unsigned int point_b,
                                 float distance_constraint);
#endif // PHYSICS_H
