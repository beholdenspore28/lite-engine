#include "physics.h"

l_verlet_body l_verlet_body_alloc(l_object object) {
  l_verlet_body verlet;
  verlet.acceleration = calloc(sizeof(*verlet.acceleration), object.count);
  verlet.position_old = calloc(sizeof(*verlet.position_old), object.count);
  verlet.is_pinned = calloc(sizeof(*verlet.is_pinned), object.count);

  verlet.bounciness = 0.2;
  verlet.friction = 0.995;

  for (unsigned int i = 0; i < object.count; i++) {
    verlet.position_old[i] = object.transform.position[i];
    verlet.acceleration[i] = vector3_zero();
    verlet.is_pinned[i] = 0;
  }

  return verlet;
}

void l_verlet_body_free(l_verlet_body verlet) {
  free(verlet.acceleration);
  free(verlet.position_old);
  free(verlet.is_pinned);
}

void l_verlet_body_update(l_object object, l_verlet_body verlet) {

  for (unsigned int i = 0; i < object.count; i++) {

    if (verlet.is_pinned[i])
      continue;

    vector3 velocity =
        vector3_subtract(object.transform.position[i], verlet.position_old[i]);

    // TODO. This is unreallistic as hell. not how friction works...
    // i mainly just did this to keep the points from moving forever.
    velocity = vector3_scale(velocity, verlet.friction);

    verlet.position_old[i] = object.transform.position[i];

    object.transform.position[i] =
        vector3_add(object.transform.position[i], velocity);

    object.transform.position[i] =
        vector3_add(object.transform.position[i], verlet.acceleration[i]);
    verlet.acceleration[i] = vector3_zero();
  }
}

void l_verlet_body_confine(l_object object, l_verlet_body verlet,
                           vector3 bounds) {

  for (unsigned int i = 0; i < object.count; i++) {

    vector3 velocity =
        vector3_subtract(object.transform.position[i], verlet.position_old[i]);

    if (object.transform.position[i].x > bounds.x) {
      object.transform.position[i].x = bounds.x;
      verlet.position_old[i].x =
          object.transform.position[i].x + velocity.x * verlet.bounciness;
    }

    if (object.transform.position[i].y > bounds.y) {
      object.transform.position[i].y = bounds.y;
      verlet.position_old[i].y =
          object.transform.position[i].y + velocity.y * verlet.bounciness;
    }

    if (object.transform.position[i].z > bounds.z) {
      object.transform.position[i].z = bounds.z;
      verlet.position_old[i].z =
          object.transform.position[i].z + velocity.z * verlet.bounciness;
    }

    if (object.transform.position[i].x < -bounds.x) {
      object.transform.position[i].x = -bounds.x;
      verlet.position_old[i].x =
          object.transform.position[i].x + velocity.x * verlet.bounciness;
    }

    if (object.transform.position[i].y < -bounds.y) {
      object.transform.position[i].y = -bounds.y;
      verlet.position_old[i].y =
          object.transform.position[i].y + velocity.y * verlet.bounciness;
    }

    if (object.transform.position[i].z < -bounds.z) {
      object.transform.position[i].z = -bounds.z;
      verlet.position_old[i].z =
          object.transform.position[i].z + velocity.z * verlet.bounciness;
    }
  }
}

void l_verlet_resolve_collisions(l_object object) {
  for (unsigned int i = 0; i < object.count; i++) {
    for (unsigned int j = 0; j < object.count; j++) {
        vector3 direction = vector3_subtract(object.transform.position[i],
            object.transform.position[j]);

        float distance = vector3_magnitude(direction);

        if (distance < 1) { // <- 1 is arbitrary for now

          vector3 correction = vector3_scale(direction, distance-1);

          object.transform.position[i] = vector3_subtract(
              object.transform.position[i], correction);

          object.transform.position[j] = vector3_add(
              object.transform.position[j], correction);
        }
    }
  }
}

void l_verlet_body_constrain_distance(l_object object, l_verlet_body verlet,
                                      unsigned int point_a,
                                      unsigned int point_b,
                                      float distance_constraint) {

  vector3 diff = vector3_subtract(object.transform.position[point_b],
                                    object.transform.position[point_a]);
  float distance = vector3_magnitude(diff);
  float adjustment = (distance_constraint - distance) / distance * 0.5;
  vector3 offset = vector3_scale(diff, adjustment);

  if (!verlet.is_pinned[point_a]) {
    object.transform.position[point_a] =
        vector3_subtract(object.transform.position[point_a], offset);
  }
  if (!verlet.is_pinned[point_b]) {
    object.transform.position[point_b] =
        vector3_add(object.transform.position[point_b], offset);
  }
}
