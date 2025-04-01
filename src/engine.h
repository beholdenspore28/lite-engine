#ifndef ENGINE_H
#define ENGINE_H

#include "blib/blib_math3d.h"

enum {
  L_ARCHETYPE_EMPTY,
  L_ARCHETYPE_CUBE,
  L_ARCHETYPE_QUAD,
};

typedef struct {
  vector3_t *position;
  vector3_t *scale;
  quaternion_t *rotation;
  float *matrix;
} l_transform_t;

typedef struct {
  l_transform_t transform;
  unsigned int count;
} l_object_t;

l_object_t l_object_alloc(unsigned int count);
void l_object_free(l_object_t object);

#endif
