#include "physics.h"

l_verlet_t l_verlet_alloc(lgl_batch_t batch) {
  l_verlet_t verlet;
  verlet.count = batch.count;
  verlet.bounce = 0.4;
  verlet.gravity = -0.0981;
  verlet.friction = 0.99;

  verlet.position_old = calloc(sizeof(*verlet.position_old), batch.count);
  verlet.is_pinned = calloc(sizeof(*verlet.is_pinned), batch.count);

  for (unsigned int i = 0; i < batch.count; i++) {
    verlet.position_old[i] = batch.transform.position[i];
    verlet.is_pinned[i] = 0;
  }

  return verlet;
}

void l_verlet_free(l_verlet_t verlet) {
  free(verlet.position_old);
  free(verlet.is_pinned);
}

void l_verlet_update(lgl_batch_t batch, l_verlet_t points) {

  for (unsigned int i = 0; i < points.count; i++) {

    if (points.is_pinned[i])
      continue;

    vector3_t velocity =
        vector3_subtract(batch.transform.position[i], points.position_old[i]);

    velocity = vector3_scale(velocity, points.friction);

    points.position_old[i] = batch.transform.position[i];
    batch.transform.position[i] = vector3_add(batch.transform.position[i], velocity);
    batch.transform.position[i].y += points.gravity;
  }
}

void l_verlet_confine(l_verlet_t verlet, lgl_batch_t batch,
                      vector3_t bounds) {

  for (unsigned int i = 0; i < batch.count; i++) {

    vector3_t velocity =
        vector3_subtract(batch.transform.position[i], verlet.position_old[i]);

    if (batch.transform.position[i].x > bounds.x) {
      batch.transform.position[i].x = bounds.x;
      verlet.position_old[i].x =
          batch.transform.position[i].x + velocity.x * verlet.bounce;
    }

    if (batch.transform.position[i].y > bounds.y) {
      batch.transform.position[i].y = bounds.y;
      verlet.position_old[i].y =
          batch.transform.position[i].y + velocity.y * verlet.bounce;
    }

    if (batch.transform.position[i].z > bounds.z) {
      batch.transform.position[i].z = bounds.z;
      verlet.position_old[i].z =
          batch.transform.position[i].z + velocity.z * verlet.bounce;
    }

    if (batch.transform.position[i].x < -bounds.x) {
      batch.transform.position[i].x = -bounds.x;
      verlet.position_old[i].x =
          batch.transform.position[i].x + velocity.x * verlet.bounce;
    }

    if (batch.transform.position[i].y < -bounds.y) {
      batch.transform.position[i].y = -bounds.y;
      verlet.position_old[i].y =
          batch.transform.position[i].y + velocity.y * verlet.bounce;
    }

    if (batch.transform.position[i].z < -bounds.z) {
      batch.transform.position[i].z = -bounds.z;
      verlet.position_old[i].z =
          batch.transform.position[i].z + velocity.z * verlet.bounce;
    }
  }
}

void l_verlet_constrain_distance(lgl_batch_t batch, l_verlet_t verlet,
                                 unsigned int point_a, unsigned int point_b,
                                 float distance_constraint) {

  vector3_t diff =
      vector3_subtract(batch.transform.position[point_b], batch.transform.position[point_a]);
  float distance = vector3_magnitude(diff);
  float adjustment = (distance_constraint - distance) / distance * 0.5;
  vector3_t offset = vector3_scale(diff, adjustment);

  if (!verlet.is_pinned[point_a]) {
    batch.transform.position[point_a] =
        vector3_subtract(batch.transform.position[point_a], offset);
  }
  if (!verlet.is_pinned[point_b]) {
    batch.transform.position[point_b] = vector3_add(batch.transform.position[point_b], offset);
  }
}
