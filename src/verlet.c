#include "physics.h"

l_verlet_t l_verlet_alloc(lgl_object_t object) {
  l_verlet_t verlet;
  verlet.count = object.count;
  verlet.bounce = 0.4;
  verlet.gravity = -0.0981;
  verlet.friction = 0.99;

  verlet.position_old = calloc(sizeof(*verlet.position_old), object.count);
  verlet.is_pinned = calloc(sizeof(*verlet.is_pinned), object.count);

  for (unsigned int i = 0; i < object.count; i++) {
    verlet.position_old[i] = object.position[i];
    verlet.is_pinned[i] = 0;
  }

  return verlet;
}

void l_verlet_free(l_verlet_t verlet) {
  free(verlet.position_old);
  free(verlet.is_pinned);
}

void l_verlet_update(lgl_object_t object, l_verlet_t points) {

  for (unsigned int i = 0; i < points.count; i++) {

    if (points.is_pinned[i])
      continue;

    vector3_t velocity =
        vector3_subtract(object.position[i], points.position_old[i]);

    velocity = vector3_scale(velocity, points.friction);

    points.position_old[i] = object.position[i];
    object.position[i] = vector3_add(object.position[i], velocity);
    object.position[i].y += points.gravity;
  }
}

void l_verlet_confine(l_verlet_t verlet, lgl_object_t object,
                      vector3_t bounds) {

  for (unsigned int i = 0; i < object.count; i++) {

    vector3_t velocity =
        vector3_subtract(object.position[i], verlet.position_old[i]);

    if (object.position[i].x > bounds.x) {
      object.position[i].x = bounds.x;
      verlet.position_old[i].x =
          object.position[i].x + velocity.x * verlet.bounce;
    }

    if (object.position[i].y > bounds.y) {
      object.position[i].y = bounds.y;
      verlet.position_old[i].y =
          object.position[i].y + velocity.y * verlet.bounce;
    }

    if (object.position[i].z > bounds.z) {
      object.position[i].z = bounds.z;
      verlet.position_old[i].z =
          object.position[i].z + velocity.z * verlet.bounce;
    }

    if (object.position[i].x < -bounds.x) {
      object.position[i].x = -bounds.x;
      verlet.position_old[i].x =
          object.position[i].x + velocity.x * verlet.bounce;
    }

    if (object.position[i].y < -bounds.y) {
      object.position[i].y = -bounds.y;
      verlet.position_old[i].y =
          object.position[i].y + velocity.y * verlet.bounce;
    }

    if (object.position[i].z < -bounds.z) {
      object.position[i].z = -bounds.z;
      verlet.position_old[i].z =
          object.position[i].z + velocity.z * verlet.bounce;
    }
  }
}

void l_verlet_constrain_distance(lgl_object_t object, l_verlet_t verlet,
                                 unsigned int point_a, unsigned int point_b,
                                 float distance_constraint) {

  vector3_t diff =
      vector3_subtract(object.position[point_b], object.position[point_a]);
  float distance = vector3_magnitude(diff);
  float adjustment = (distance_constraint - distance) / distance * 0.5;
  vector3_t offset = vector3_scale(diff, adjustment);

  if (!verlet.is_pinned[point_a]) {
    object.position[point_a] =
        vector3_subtract(object.position[point_a], offset);
  }
  if (!verlet.is_pinned[point_b]) {
    object.position[point_b] = vector3_add(object.position[point_b], offset);
  }
}

