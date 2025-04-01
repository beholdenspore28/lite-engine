#include "physics.h"

l_verlet_t l_verlet_alloc(l_object_t object) {
  l_verlet_t verlet;
  verlet.count = object.count;
  verlet.bounce = 0.4;
  verlet.gravity = -0.05;
  verlet.friction = 0.9;

  verlet.position_old =
      calloc(sizeof(*verlet.position_old), object.count);
  verlet.is_pinned = calloc(sizeof(*verlet.is_pinned), object.count);

  for (unsigned int i = 0; i < object.count; i++) {
    verlet.position_old[i] = object.transform.position[i];
    verlet.is_pinned[i] = 0;
  }

  return verlet;
}

void l_verlet_free(l_verlet_t verlet) {
  free(verlet.position_old);
  free(verlet.is_pinned);
}

void l_verlet_update(l_object_t object, l_verlet_t points) {

  for (unsigned int i = 0; i < points.count; i++) {

    if (points.is_pinned[i])
      continue;

    vector3_t velocity = vector3_subtract(object.transform.position[i],
                                          points.position_old[i]);

    velocity = vector3_scale(velocity, points.friction);

    points.position_old[i] = object.transform.position[i];
    object.transform.position[i] =
        vector3_add(object.transform.position[i], velocity);
    object.transform.position[i].y += points.gravity;
  }
}

void l_verlet_confine(l_object_t object, l_verlet_t verlet, vector3_t bounds) {

  for (unsigned int i = 0; i < object.count; i++) {

    vector3_t velocity = vector3_subtract(object.transform.position[i],
                                          verlet.position_old[i]);

    if (object.transform.position[i].x > bounds.x) {
      object.transform.position[i].x = bounds.x;
      verlet.position_old[i].x =
          object.transform.position[i].x + velocity.x * verlet.bounce;
    }

    if (object.transform.position[i].y > bounds.y) {
      object.transform.position[i].y = bounds.y;
      verlet.position_old[i].y =
          object.transform.position[i].y + velocity.y * verlet.bounce;
    }

    if (object.transform.position[i].z > bounds.z) {
      object.transform.position[i].z = bounds.z;
      verlet.position_old[i].z =
          object.transform.position[i].z + velocity.z * verlet.bounce;
    }

    if (object.transform.position[i].x < -bounds.x) {
      object.transform.position[i].x = -bounds.x;
      verlet.position_old[i].x =
          object.transform.position[i].x + velocity.x * verlet.bounce;
    }

    if (object.transform.position[i].y < -bounds.y) {
      object.transform.position[i].y = -bounds.y;
      verlet.position_old[i].y =
          object.transform.position[i].y + velocity.y * verlet.bounce;
    }

    if (object.transform.position[i].z < -bounds.z) {
      object.transform.position[i].z = -bounds.z;
      verlet.position_old[i].z =
          object.transform.position[i].z + velocity.z * verlet.bounce;
    }
  }
}

void l_verlet_constrain_distance(l_object_t object, l_verlet_t verlet,
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
