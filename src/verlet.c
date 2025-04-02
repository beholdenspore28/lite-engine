#include "physics.h"

l_verlet_body l_verlet_body_alloc(l_object_t object) {
  l_verlet_body verlet;
  verlet.acceleration = calloc(sizeof(*verlet.acceleration), object.count);
  verlet.position_old = calloc(sizeof(*verlet.position_old), object.count);
  verlet.is_pinned = calloc(sizeof(*verlet.is_pinned), object.count);

  for (unsigned int i = 0; i < object.count; i++) {
    verlet.position_old[i] = object.transform.position[i];
    verlet.is_pinned[i] = 0;
  }

  return verlet;
}

void l_verlet_body_free(l_verlet_body verlet) {
  free(verlet.acceleration);
  free(verlet.position_old);
  free(verlet.is_pinned);
}

void l_verlet_body_update(l_object_t object, l_verlet_body verlet) {

  for (unsigned int i = 0; i < object.count; i++) {

    if (verlet.is_pinned[i])
      continue;

    const vector3_t velocity =
        vector3_subtract(object.transform.position[i], verlet.position_old[i]);

    verlet.position_old[i] = object.transform.position[i];

    object.transform.position[i] =
        vector3_add(object.transform.position[i], velocity);

    object.transform.position[i] =
        vector3_add(object.transform.position[i], verlet.acceleration[i]);
    verlet.acceleration[i] = vector3_zero();
  }
}

void l_verlet_body_confine(l_object_t object, l_verlet_body verlet, vector3_t bounds) {

  for (unsigned int i = 0; i < object.count; i++) {

    vector3_t velocity =
        vector3_subtract(object.transform.position[i], verlet.position_old[i]);

    if (object.transform.position[i].x > bounds.x) {
      object.transform.position[i].x = bounds.x;
      verlet.position_old[i].x = object.transform.position[i].x + velocity.x;
    }

    if (object.transform.position[i].y > bounds.y) {
      object.transform.position[i].y = bounds.y;
      verlet.position_old[i].y = object.transform.position[i].y + velocity.y;
    }

    if (object.transform.position[i].z > bounds.z) {
      object.transform.position[i].z = bounds.z;
      verlet.position_old[i].z = object.transform.position[i].z + velocity.z;
    }

    if (object.transform.position[i].x < -bounds.x) {
      object.transform.position[i].x = -bounds.x;
      verlet.position_old[i].x = object.transform.position[i].x + velocity.x;
    }

    if (object.transform.position[i].y < -bounds.y) {
      object.transform.position[i].y = -bounds.y;
      verlet.position_old[i].y = object.transform.position[i].y + velocity.y;
    }

    if (object.transform.position[i].z < -bounds.z) {
      object.transform.position[i].z = -bounds.z;
      verlet.position_old[i].z = object.transform.position[i].z + velocity.z;
    }
  }
}

void l_verlet_body_constrain_distance(l_object_t object, l_verlet_body verlet,
                                 unsigned int point_a, unsigned int point_b,
                                 float distance_constraint) {

  vector3_t diff = vector3_subtract(object.transform.position[point_b],
                                    object.transform.position[point_a]);
  float distance = vector3_magnitude(diff);
  float adjustment = (distance_constraint - distance) / distance * 0.5;
  vector3_t offset = vector3_scale(diff, adjustment);

  if (!verlet.is_pinned[point_a]) {
    object.transform.position[point_a] =
        vector3_subtract(object.transform.position[point_a], offset);
  }
  if (!verlet.is_pinned[point_b]) {
    object.transform.position[point_b] =
        vector3_add(object.transform.position[point_b], offset);
  }
}
