#ifndef ENGINE_H
#define ENGINE_H

#include "blib/blib_math3d.h"

typedef struct {
  vector3_t *position;
  vector3_t *scale;
  quaternion_t *rotation;
  unsigned int count;
} l_transform_t;

#endif
