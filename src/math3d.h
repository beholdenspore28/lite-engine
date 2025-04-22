#ifndef MATH_3D_H
#define MATH_3D_H

#include "mathf.h"

// For added convenience, you can use printf(vector3_TO_STRING(vec)) .
#define VECTOR2_TO_STRING(v) "{%f, %f}", v.x, v.y
#define VECTOR3_TO_STRING(v) "{%f, %f, %f}", v.x, v.y, v.z
#define VECTOR4_TO_STRING(v) "{%f, %f, %f, %f}", q.x, q.y, q.z, q.w

#define MATH_3D_API static inline

// simple vector type
typedef struct {
  float x, y;
} vector2;

// simple vector type
typedef struct {
  float x, y, z;
} vector3;

// simple vector/quaternion type
typedef struct {
  float x, y, z, w;
} vector4;

MATH_3D_API void mat4_print(float *mat) {
  printf("\n-----------------------------\n");
  for (int j = 0; j < 4; j++) {
    printf("%.2f\t", mat[j]);
  }
  putchar('\n');
  for (int j = 4; j < 8; j++) {
    printf("%.2f\t", mat[j]);
  }
  putchar('\n');
  for (int j = 8; j < 12; j++) {
    printf("%.2f\t", mat[j]);
  }
  putchar('\n');
  for (int j = 12; j < 16; j++) {
    printf("%.2f\t", mat[j]);
  }
  putchar('\n');
}

MATH_3D_API void mat4_identity(float *m) {
  m[0] = 1.0;
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = 1.0;
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = 0.0;
  m[9] = 0.0;
  m[10] = 1.0;
  m[11] = 0.0;
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;
}

/*Multiplies a 4x4 matrix with another 4x4 matrix*/
MATH_3D_API void mat4_multiply(float *result, const float *a, const float *b) {

  // row 0
  result[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
  result[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
  result[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
  result[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];

  // row 1
  result[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
  result[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
  result[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
  result[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];

  // row 2
  result[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
  result[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
  result[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
  result[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];

  // row 3
  result[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
  result[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
  result[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
  result[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
}

// subtracts the vector b from a and stores the result in a
MATH_3D_API void vector2_take(vector2 *a, vector2 b) {
  a->x -= b.x;
  a->y -= b.y;
}

// subtracts the vector b from a and stores the result in a
MATH_3D_API void vector3_take(vector3 *a, vector3 b) {
  a->x -= b.x;
  a->y -= b.y;
  a->z -= b.z;
}

// subtracts the vector b from a and stores the result in a
MATH_3D_API void vector4_take(vector4 *a, vector4 b) {
  a->x -= b.x;
  a->y -= b.y;
  a->z -= b.z;
  a->w -= b.w;
}

// adds the vector b to a and stores the result in a
MATH_3D_API void vector2_give(vector2 *a, vector2 b) {
  a->x += b.x;
  a->y += b.y;
}

// adds the vector b to a and stores the result in a
MATH_3D_API void vector3_give(vector3 *a, vector3 b) {
  a->x += b.x;
  a->y += b.y;
  a->z += b.z;
}

// adds the vector b to a and stores the result in a
MATH_3D_API void vector4_give(vector4 *a, vector4 b) {
  a->x += b.x;
  a->y += b.y;
  a->z += b.z;
  a->w += b.w;
}

// returns the sum of a and b
MATH_3D_API vector2 vector2_add(vector2 a, vector2 b) {
  return (vector2){
      .x = a.x + b.x,
      .y = a.y + b.y,
  };
}

// returns the sum of a and b
MATH_3D_API vector3 vector3_add(vector3 a, vector3 b) {
  return (vector3){
      .x = a.x + b.x,
      .y = a.y + b.y,
      .z = a.z + b.z,
  };
}

// returns the sum of a and b
MATH_3D_API vector4 vector4_add(vector4 a, vector4 b) {
  return (vector4){
      .x = a.x + b.x,
      .y = a.y + b.y,
      .z = a.z + b.z,
  };
}

// returns the difference between a and b
MATH_3D_API vector2 vector2_sub(vector2 a, vector2 b) {
  return (vector2){
      .x = a.x - b.x,
      .y = a.y - b.y,
  };
}

// returns the difference between a and b
MATH_3D_API vector3 vector3_sub(vector3 a, vector3 b) {
  return (vector3){
      .x = a.x - b.x,
      .y = a.y - b.y,
      .z = a.z - b.z,
  };
}

// returns the difference between a and b
MATH_3D_API vector4 vector4_sub(vector4 a, vector4 b) {
  return (vector4){
      .x = a.x - b.x,
      .y = a.y - b.y,
      .z = a.z - b.z,
  };
}

MATH_3D_API vector2 vector2_scaled(const vector2 v, const float s) {
  return (vector2){
      v.x * s,
      v.y * s,
  };
}

MATH_3D_API vector3 vector3_scaled(const vector3 v, const float s) {
  return (vector3){
      v.x * s,
      v.y * s,
      v.z * s,
  };
}

MATH_3D_API vector4 vector4_scaled(const vector4 v, float scalar) {
  return (vector4){
      v.x * scalar,
      v.y * scalar,
      v.z * scalar,
      v.w * scalar,
  };
}

// scale vector v by scalar s and store the result in v
MATH_3D_API void vector2_scale(vector2 *v, const float s) {
  v->x *= s;
  v->y *= s;
}

// scale vector v by scalar s and store the result in v
MATH_3D_API void vector3_scale(vector3 *v, const float s) {
  v->x *= s;
  v->y *= s;
  v->z *= s;
}

MATH_3D_API void vector4_scale(vector4 *v, float scalar) {
  v->x *= scalar;
  v->y *= scalar;
  v->z *= scalar;
  v->w *= scalar;
}

// use this instead of vector2_magnitude when exact magnitude calculations are
// not needed
MATH_3D_API float vector2_square_magnitude(const vector2 v) {
  return v.x * v.x + v.y * v.y;
}

// use this instead of vector3_magnitude when exact magnitude calculations are
// not needed
MATH_3D_API float vector3_square_magnitude(const vector3 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

// use this instead of vector4_magnitude when exact magnitude calculations are
// not needed
MATH_3D_API float vector4_square_magnitude(const vector4 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

// Returns the magnitude (length) of v. Note this uses sqrt so it may slow
// things down, use vector2_square_magnitude for faster (less accurate) results.
MATH_3D_API float vector2_magnitude(const vector2 v) {
  return sqrtf(vector2_square_magnitude(v));
}

// Returns the magnitude (length) of v. Note this uses sqrt so it may slow
// things down, use vector3_square_magnitude for faster (less accurate) results.
MATH_3D_API float vector3_magnitude(const vector3 v) {
  return sqrtf(vector3_square_magnitude(v));
}

// Returns the magnitude (length) of v. Note this uses sqrt so it may slow
// things down, use vector4_square_magnitude for faster (less accurate) results.
MATH_3D_API float vector4_magnitude(const vector4 v) {
  return sqrtf(vector4_square_magnitude(v));
}

// Returns a vector which points in the same direction as v, but has a magnitude
// of 1
MATH_3D_API void vector2_normalize(vector2 *v) {
  float m = vector2_magnitude(*v);
  vector2_scale(v, 1 / m);
}

// Returns a vector which points in the same direction as v, but has a magnitude
// of 1
MATH_3D_API void vector3_normalize(vector3 *v) {
  float m = vector3_magnitude(*v);
  if (m == 0) { // prevent NAN values
    *v = (vector3){0, 0, 0};
    return;
  }
  vector3_scale(v, 1 / m);
}

// Returns a vector which points in the same direction as v, but has a magnitude
// of 1
MATH_3D_API void vector4_normalize(vector4 *v) {
  float m = vector4_magnitude(*v);
  vector4_scale(v, 1 / m);
}

// faster (less accurate) version of vector2_distance
MATH_3D_API float vector2_square_distance(const vector2 a, const vector2 b) {
  return vector2_square_magnitude(vector2_sub(b, a));
}

// faster (less accurate) version of vector3_distance
MATH_3D_API float vector3_square_distance(const vector3 a, const vector3 b) {
  return vector3_square_magnitude(vector3_sub(b, a));
}

// faster (less accurate) version of vector4_distance
MATH_3D_API float vector4_square_distance(const vector4 a, const vector4 b) {
  return vector4_square_magnitude(vector4_sub(b, a));
}

// Returns the distance between a and b.
MATH_3D_API float vector2_distance(const vector2 a, const vector2 b) {
  return vector2_magnitude(vector2_sub(b, a));
}

// Returns the distance between a and b.
MATH_3D_API float vector3_distance(const vector3 a, const vector3 b) {
  return vector3_magnitude(vector3_sub(b, a));
}

// Returns the distance between a and b.
MATH_3D_API float vector4_distance(const vector4 a, const vector4 b) {
  return vector4_magnitude(vector4_sub(b, a));
}

// Returns the dot product of a and b. 1 if a is close to b, -1 if a is far from
// b
MATH_3D_API float vector2_dot(const vector2 a, const vector2 b) {
  return a.x * b.x + a.y * b.y;
}

// Returns the dot product of a and b. 1 if a is close to b, -1 if a is far from
// b
MATH_3D_API float vector3_dot(const vector3 a, const vector3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Returns the dot product of a and b. 1 if a is close to b, -1 if a is far from
// b
MATH_3D_API float vector4_dot(const vector4 a, const vector4 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

// returns the vector that is perpendicular to both a and b.
MATH_3D_API vector3 vector3_cross(vector3 a, vector3 b) {
  return (vector3){.x = (a.y * b.z) - (a.z * b.y),
                   .y = -((a.x * b.z) - (a.z * b.x)),
                   .z = (a.x * b.y) - (a.y * b.x)};
}

// Returns the point that is t% of the way between a and b.
MATH_3D_API vector2 vector2_lerp(vector2 a, vector2 b, float t) {
  return (vector2){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
  };
}

// Returns the point that is t% of the way between a and b.
MATH_3D_API vector3 vector3_lerp(vector3 a, vector3 b, float t) {
  return (vector3){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
      .z = a.z + (b.z - a.z) * t,
  };
}

// Returns the point that is t% of the way between a and b.
MATH_3D_API vector4 vector4_lerp(vector4 a, vector4 b, float t) {
  return (vector4){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
      .z = a.z + (b.z - a.z) * t,
      .w = a.w + (b.w - a.w) * t,
  };
}

// Returns a vector with the smallest components from both a and b.
MATH_3D_API vector2 vector2_min(vector2 a, vector2 b) {
  return (vector2){
      .x = a.x <= b.x ? a.x : b.x,
      .y = a.y <= b.y ? a.y : b.y,
  };
}

// Returns a vector with the smallest components from both a and b.
MATH_3D_API vector3 vector3_min(vector3 a, vector3 b) {
  return (vector3){
      .x = a.x <= b.x ? a.x : b.x,
      .y = a.y <= b.y ? a.y : b.y,
      .z = a.z <= b.z ? a.z : b.z,
  };
}

// Returns a vector with the smallest components from both a and b.
MATH_3D_API vector4 vector4_min(vector4 a, vector4 b) {
  return (vector4){
      .x = a.x <= b.x ? a.x : b.x,
      .y = a.y <= b.y ? a.y : b.y,
      .z = a.z <= b.z ? a.z : b.z,
      .w = a.w <= b.w ? a.w : b.w,
  };
}

// Returns a vector with the biggest components from both a and b.
MATH_3D_API vector2 vector2_max(vector2 a, vector2 b) {
  return (vector2){
      .x = a.x >= b.x ? a.x : b.x,
      .y = a.y >= b.y ? a.y : b.y,
  };
}

// Returns a vector with the biggest components from both a and b.
MATH_3D_API vector3 vector3_max(vector3 a, vector3 b) {
  return (vector3){
      .x = a.x >= b.x ? a.x : b.x,
      .y = a.y >= b.y ? a.y : b.y,
      .z = a.z >= b.z ? a.z : b.z,
  };
}

// Returns a vector with the biggest components from both a and b.
MATH_3D_API vector4 vector4_max(vector4 a, vector4 b) {
  return (vector4){
      .x = a.x >= b.x ? a.x : b.x,
      .y = a.y >= b.y ? a.y : b.y,
      .z = a.z >= b.z ? a.z : b.z,
      .w = a.w >= b.w ? a.w : b.w,
  };
}

MATH_3D_API vector2 vector2_noise(unsigned int seed, float range) {
  float d, x, y;
  int i = 0;
  do {
    x = mathf_noise2(i + seed + 1, i + seed) * (2.0) - 1.0;
    y = mathf_noise2(i + seed, i + seed + 1) * (2.0) - 1.0;
    d = x * x + y * y;
    i++;
  } while (d > 1.0);
  return vector2_scaled((vector2){x, y}, range);
}

MATH_3D_API vector3 vector3_noise(unsigned int seed, float range) {
  float d, x, y, z;
  int i = 0;
  do {
    x = mathf_noise3(i + seed + 1, i + seed, i + seed) * (2.0) - 1.0;
    y = mathf_noise3(i + seed, i + seed + 1, i + seed) * (2.0) - 1.0;
    z = mathf_noise3(i + seed, i + seed, i + seed + 1) * (2.0) - 1.0;
    d = x * x + y * y + z * z;
    i++;
  } while (d > 1.0);
  return vector3_scaled((vector3){x, y, z}, range);
}

// Returns true if a is equal to b.
MATH_3D_API int vector2_equal(vector2 a, vector2 b) {
  return fabs(a.x - b.x) <= MATHF_EPSILON && fabs(a.y - b.y) <= MATHF_EPSILON;
}

// Returns true if a is equal to b.
MATH_3D_API int vector3_equal(vector3 a, vector3 b) {
  return fabs(a.x - b.x) <= MATHF_EPSILON && fabs(a.y - b.y) <= MATHF_EPSILON &&
         fabs(a.z - b.z) <= MATHF_EPSILON;
}

// Returns true if a is equal to b.
MATH_3D_API int vector4_equal(vector4 a, vector4 b) {
  return fabs(a.x - b.x) <= MATHF_EPSILON && fabs(a.y - b.y) <= MATHF_EPSILON &&
         fabs(a.z - b.z) <= MATHF_EPSILON && fabs(a.w - b.w) <= MATHF_EPSILON;
}

// Multiply q1 with q2 (perform rotation on q1 by q2)
MATH_3D_API vector4 quaternion_multiply(vector4 q1, vector4 q2) {
  vector4 ret = {0};
  ret.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  ret.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
  ret.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
  ret.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
  return ret;
}

MATH_3D_API vector4 quaternion_conjugate(vector4 q) {
  return (vector4){-q.x, -q.y, -q.z, q.w};
}

// Rotates v by q
MATH_3D_API vector3 vector3_rotate(vector3 v, vector4 q) {
  vector4 ret = (vector4){v.x, v.y, v.z, 0.0};
  ret =
      quaternion_multiply(quaternion_multiply(q, ret), quaternion_conjugate(q));
  return (vector3){ret.x, ret.y, ret.z};
}

MATH_3D_API vector4 quaternion_from_euler(vector3 euler_angles) {
  vector4 q;

  float cos_roll = cosf(euler_angles.x * 0.5f),
        sin_roll = sinf(euler_angles.x * 0.5f),
        cos_pitch = cosf(euler_angles.y * 0.5f),
        sin_pitch = sinf(euler_angles.y * 0.5f),
        cos_yaw = cosf(euler_angles.z * 0.5f),
        sin_yaw = sinf(euler_angles.z * 0.5f);

  q.w = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;
  q.x = sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw;
  q.y = cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw;
  q.z = cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw;

  return q;
}

MATH_3D_API vector4 quaternion_rotate_euler(vector4 q, vector3 euler_angles) {
  return quaternion_multiply(q, quaternion_from_euler(euler_angles));
}

MATH_3D_API float *quaternion_to_mat4(vector4 q, float *mat) {
  float xx = q.x * q.x, xy = q.x * q.y, xz = q.x * q.z, xw = q.x * q.w,

        yy = q.y * q.y, yz = q.y * q.z, yw = q.y * q.w,

        zz = q.z * q.z, zw = q.z * q.w;

  mat[0] = 1 - 2 * (yy + zz);
  mat[4] = 2 * (xy - zw);
  mat[8] = 2 * (xz + yw);

  mat[1] = 2 * (xy + zw);
  mat[5] = 1 - 2 * (xx + zz);
  mat[9] = 2 * (yz - xw);

  mat[2] = 2 * (xz - yw);
  mat[6] = 2 * (yz + xw);
  mat[10] = 1 - 2 * (xx + yy);

  mat[12] = mat[13] = mat[14] = mat[3] = mat[7] = mat[11] = 0;
  mat[15] = 1;

  return mat;
}

#endif // MATH_3D_H
