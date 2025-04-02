#ifndef ENGINE_H
#define ENGINE_H

#include "blib/blib_math3d.h"

enum {
  L_ARCHETYPE_EMPTY,
  L_ARCHETYPE_CUBE,
  L_ARCHETYPE_QUAD,
};

typedef struct {
  vector3 *position;
  vector3 *scale;
  quaternion *rotation;
  float *matrix;
} l_transform;

static inline vector3 l_object_forward(l_transform transform,
                                       unsigned int index, float magnitude) {
  return vector3_rotate(vector3_forward(magnitude), transform.rotation[index]);
}

static inline vector3 l_object_back(l_transform transform, unsigned int index,
                                    float magnitude) {
  return vector3_rotate(vector3_back(magnitude), transform.rotation[index]);
}

static inline vector3 l_object_right(l_transform transform, unsigned int index,
                                     float magnitude) {
  return vector3_rotate(vector3_right(magnitude), transform.rotation[index]);
}

static inline vector3 l_object_left(l_transform transform, unsigned int index,
                                    float magnitude) {
  return vector3_rotate(vector3_left(magnitude), transform.rotation[index]);
}

static inline vector3 l_object_up(l_transform transform, unsigned int index,
                                  float magnitude) {
  return vector3_rotate(vector3_up(magnitude), transform.rotation[index]);
}

static inline vector3 l_object_down(l_transform transform, unsigned int index,
                                    float magnitude) {
  return vector3_rotate(vector3_down(magnitude), transform.rotation[index]);
}

typedef struct {
  l_transform transform;
  unsigned int count;
} l_object;

l_object l_object_alloc(unsigned int count);
void l_object_free(l_object object);

#endif
