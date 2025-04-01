#include "engine.h"

l_object_t l_object_alloc(unsigned int count) {

  l_object_t object;
  object.transform.scale = calloc(sizeof(*object.transform.scale), count);
  object.transform.position = calloc(sizeof(*object.transform.position), count);
  object.transform.rotation = calloc(sizeof(*object.transform.rotation), count);
  object.transform.matrix =
      calloc(sizeof(*object.transform.matrix) * 16, count);
  object.count = count;

  for (unsigned int j = 0; j < count; j++) {
    object.transform.scale[j] = vector3_one(1.0);
    object.transform.position[j] = vector3_zero();
    object.transform.rotation[j] = quaternion_identity();
  }
  return object;
}

void l_object_free(l_object_t object) {
  free(object.transform.matrix);
  free(object.transform.scale);
  free(object.transform.position);
  free(object.transform.rotation);
}
