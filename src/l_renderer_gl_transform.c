#include "l_renderer_gl.h"

l_renderer_gl_transform l_renderer_gl_transform_create() {
  l_renderer_gl_transform t;
  t.scale = BLIB_VEC3F_ONE;
  t.eulerAngles = BLIB_VEC3F_ZERO;
  t.position = BLIB_VEC3F_ZERO;
  return t;
}

blib_mat4_t l_renderer_gl_transform_GetMatrix(l_renderer_gl_transform *t) {
  // translation
  blib_mat4_t translationMat = blib_mat4_translateVec3(t->position);

  // rotation
  blib_mat4_t p = blib_mat4_rotate(t->eulerAngles.x, BLIB_VEC3F_RIGHT);
  blib_mat4_t y = blib_mat4_rotate(t->eulerAngles.y, BLIB_VEC3F_UP);
  blib_mat4_t r = blib_mat4_rotate(t->eulerAngles.z, BLIB_VEC3F_FORWARD);
  blib_mat4_t rotationMat = blib_mat4_multiply(blib_mat4_multiply(r, y), p);

  // scale
  blib_mat4_t scaleMat = blib_mat4_scale(t->scale);

  // TRS = model matrix
  // Remember! Matrix mult is done in reverse order!
  blib_mat4_t modelMat = blib_mat4_multiply(rotationMat, translationMat);
  modelMat = blib_mat4_multiply(scaleMat, modelMat);

  return modelMat;
}

// TODO this might be an inefficient way to get directions.
// consider using the cross product of forward and up
blib_vec3f_t l_renderer_gl_transform_getLocalForward(l_renderer_gl_transform *t) {
  blib_mat4_t m = l_renderer_gl_transform_GetMatrix(t);
  return (blib_vec3f_t){.x = m.elements[2], .y = m.elements[6], .z = m.elements[10]};
}

blib_vec3f_t l_renderer_gl_transform_getLocalUp(l_renderer_gl_transform *t) {
  blib_mat4_t m = l_renderer_gl_transform_GetMatrix(t);
  return (blib_vec3f_t){.x = m.elements[1], .y = m.elements[5], .z = m.elements[9]};
}

blib_vec3f_t l_renderer_gl_transform_getLocalRight(l_renderer_gl_transform *t) {
  blib_mat4_t m = l_renderer_gl_transform_GetMatrix(t);
  return (blib_vec3f_t){.x = m.elements[0], .y = m.elements[4], .z = m.elements[8]};
}

// TODO? move this func to blib as a general euler rotation func?
void l_renderer_gl_transform_rotate(l_renderer_gl_transform *t, blib_vec3f_t rotation) {
  t->eulerAngles = blib_vec3f_add(t->eulerAngles, rotation);
  t->eulerAngles.x = blib_mathf_wrapAngle(t->eulerAngles.x);
  t->eulerAngles.y = blib_mathf_wrapAngle(t->eulerAngles.y);
  t->eulerAngles.z = blib_mathf_wrapAngle(t->eulerAngles.z);
}
