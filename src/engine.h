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

static inline vector3_t l_object_forward(l_transform_t transform,
                                         unsigned int index, float magnitude) {
  return vector3_rotate(vector3_forward(magnitude), transform.rotation[index]);
}

static inline vector3_t l_object_back(l_transform_t transform,
                                      unsigned int index, float magnitude) {
  return vector3_rotate(vector3_back(magnitude), transform.rotation[index]);
}

static inline vector3_t l_object_right(l_transform_t transform,
                                       unsigned int index, float magnitude) {
  return vector3_rotate(vector3_right(magnitude), transform.rotation[index]);
}

static inline vector3_t l_object_left(l_transform_t transform,
                                      unsigned int index, float magnitude) {
  return vector3_rotate(vector3_left(magnitude), transform.rotation[index]);
}

static inline vector3_t l_object_up(l_transform_t transform, unsigned int index,
                                    float magnitude) {
  return vector3_rotate(vector3_up(magnitude), transform.rotation[index]);
}

static inline vector3_t l_object_down(l_transform_t transform,
                                      unsigned int index, float magnitude) {
  return vector3_rotate(vector3_down(magnitude), transform.rotation[index]);
}

typedef struct {
  l_transform_t transform;
  unsigned int count;
} l_object_t;

l_object_t l_object_alloc(unsigned int count);
void l_object_free(l_object_t object);

#endif
