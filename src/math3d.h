#ifndef SIMPLE_3D_H
#define SIMPLE_3D_H

#include <math.h>

// For added convenience, you can use printf(SV3_TO_STRING(vec)) .
#define SV2_TO_STRING(v) "{%f, %f}", v.x, v.y
#define SV3_TO_STRING(v) "{%f, %f, %f}", v.x, v.y, v.z
#define SV4_TO_STRING(v) "{%f, %f, %f, %f}", q.x, q.y, q.z, q.w

// You can set this to your floating point epsilon of choice.
// Though as it gets smaller, it also loses accuracy.
#define SV__FLOAT_EPSILON (0.001)

#define SV_API static inline

// simple vector type
typedef struct {
  float x;
  float y;
} sv2;

// simple vector type
typedef struct {
  float x;
  float y;
  float z;
} sv3;

// simple vector/quaternion type
typedef struct {
  float x;
  float y;
  float z;
  float w;
} sv4;

// subtracts the vector b from a and stores the result in a
SV_API void sv2_take(sv2 *a, sv2 b) {
  a->x -= b.x;
  a->y -= b.y;
}

// subtracts the vector b from a and stores the result in a
SV_API void sv3_take(sv3 *a, sv3 b) {
  a->x -= b.x;
  a->y -= b.y;
  a->z -= b.z;
}

// subtracts the vector b from a and stores the result in a
SV_API void sv4_take(sv4 *a, sv4 b) {
  a->x -= b.x;
  a->y -= b.y;
  a->z -= b.z;
  a->w -= b.w;
}

// adds the vector b to a and stores the result in a
SV_API void sv2_give(sv2 *a, sv2 b) {
  a->x += b.x;
  a->y += b.y;
}

// adds the vector b to a and stores the result in a
SV_API void sv3_give(sv3 *a, sv3 b) {
  a->x += b.x;
  a->y += b.y;
  a->z += b.z;
}

// adds the vector b to a and stores the result in a
SV_API void sv4_give(sv4 *a, sv4 b) {
  a->x += b.x;
  a->y += b.y;
  a->z += b.z;
  a->w += b.w;
}

// returns the sum of a and b
SV_API sv2 sv2_add(sv2 a, sv2 b) {
  return (sv2){
      .x = a.x + b.x,
      .y = a.y + b.y,
  };
}

// returns the sum of a and b
SV_API sv3 sv3_add(sv3 a, sv3 b) {
  return (sv3){
      .x = a.x + b.x,
      .y = a.y + b.y,
      .z = a.z + b.z,
  };
}

// returns the sum of a and b
SV_API sv4 sv4_add(sv4 a, sv4 b) {
  return (sv4){
      .x = a.x + b.x,
      .y = a.y + b.y,
      .z = a.z + b.z,
  };
}

// returns the difference between a and b
SV_API sv2 sv2_sub(sv2 a, sv2 b) {
  return (sv2){
      .x = a.x - b.x,
      .y = a.y - b.y,
  };
}

// returns the difference between a and b
SV_API sv3 sv3_sub(sv3 a, sv3 b) {
  return (sv3){
      .x = a.x - b.x,
      .y = a.y - b.y,
      .z = a.z - b.z,
  };
}

// returns the difference between a and b
SV_API sv4 sv4_sub(sv4 a, sv4 b) {
  return (sv4){
      .x = a.x - b.x,
      .y = a.y - b.y,
      .z = a.z - b.z,
  };
}

SV_API sv2 sv2_scaled(const sv2 v, const float s) {
  return (sv2) {
      v.x * s,
      v.y * s,
  };
}

SV_API sv3 sv3_scaled(const sv3 v, const float s) {
  return (sv3) {
      v.x * s,
      v.y * s,
      v.z * s,
  };
}

SV_API sv4 sv4_scaled(const sv4 v, float scalar) {
  return (sv4) {
    v.x * scalar,
    v.y * scalar,
    v.z * scalar,
    v.w * scalar,
  };
}

// scale vector v by scalar s and store the result in v
SV_API void sv2_scale(sv2 *v, const float s) {
  v->x *= s;
  v->y *= s;
}

// scale vector v by scalar s and store the result in v
SV_API void sv3_scale(sv3 *v, const float s) {
  v->x *= s;
  v->y *= s;
  v->z *= s;
}

SV_API void sv4_scale(sv4 *v, float scalar) {
  v->x *= scalar;
  v->y *= scalar;
  v->z *= scalar;
  v->w *= scalar;
}

// use this instead of sv2_magnitude when exact magnitude calculations are not
// needed
SV_API float sv2_square_magnitude(const sv2 v) { return v.x * v.x + v.y * v.y; }

// use this instead of sv3_magnitude when exact magnitude calculations are not
// needed
SV_API float sv3_square_magnitude(const sv3 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

// use this instead of sv4_magnitude when exact magnitude calculations are not
// needed
SV_API float sv4_square_magnitude(const sv4 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

// Returns the magnitude (length) of v. Note this uses sqrt so it may slow
// things down, use sv2_square_magnitude for faster (less accurate) results.
SV_API float sv2_magnitude(const sv2 v) {
  return sqrtf(sv2_square_magnitude(v));
}

// Returns the magnitude (length) of v. Note this uses sqrt so it may slow
// things down, use sv3_square_magnitude for faster (less accurate) results.
SV_API float sv3_magnitude(const sv3 v) {
  return sqrtf(sv3_square_magnitude(v));
}

// Returns the magnitude (length) of v. Note this uses sqrt so it may slow
// things down, use sv4_square_magnitude for faster (less accurate) results.
SV_API float sv4_magnitude(const sv4 v) {
  return sqrtf(sv4_square_magnitude(v));
}

// Returns a vector which points in the same direction as v, but has a magnitude
// of 1
SV_API void sv2_normalize(sv2 *v) {
  float m = sv2_magnitude(*v);
  sv2_scale(v, 1 / m);
}

// Returns a vector which points in the same direction as v, but has a magnitude
// of 1
SV_API void sv3_normalize(sv3 *v) {
  float m = sv3_magnitude(*v);
  if (m == 0) { // prevent NAN values
    *v = (sv3){0,0,0};
    return;    
  }
  sv3_scale(v, 1/m);
}

// Returns a vector which points in the same direction as v, but has a magnitude
// of 1
SV_API void sv4_normalize(sv4 *v) {
  float m = sv4_magnitude(*v);
  sv4_scale(v, 1 / m);
}

// faster (less accurate) version of sv2_distance
SV_API float sv2_square_distance(const sv2 a, const sv2 b) {
  return sv2_square_magnitude(sv2_sub(b, a));
}

// faster (less accurate) version of sv3_distance
SV_API float sv3_square_distance(const sv3 a, const sv3 b) {
  return sv3_square_magnitude(sv3_sub(b, a));
}

// faster (less accurate) version of sv4_distance
SV_API float sv4_square_distance(const sv4 a, const sv4 b) {
  return sv4_square_magnitude(sv4_sub(b, a));
}

// Returns the distance between a and b.
SV_API float sv2_distance(const sv2 a, const sv2 b) {
  return sv2_magnitude(sv2_sub(b, a));
}

// Returns the distance between a and b.
SV_API float sv3_distance(const sv3 a, const sv3 b) {
  return sv3_magnitude(sv3_sub(b, a));
}

// Returns the distance between a and b.
SV_API float sv4_distance(const sv4 a, const sv4 b) {
  return sv4_magnitude(sv4_sub(b, a));
}

// Returns the dot product of a and b. 1 if a is close to b, -1 if a is far from
// b
SV_API float sv2_dot(const sv2 a, const sv2 b) { return a.x * b.x + a.y * b.y; }

// Returns the dot product of a and b. 1 if a is close to b, -1 if a is far from
// b
SV_API float sv3_dot(const sv3 a, const sv3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Returns the dot product of a and b. 1 if a is close to b, -1 if a is far from
// b
SV_API float sv4_dot(const sv4 a, const sv4 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

// returns the vector that is perpendicular to both a and b.
SV_API sv3 sv3_cross(sv3 a, sv3 b) {
  return (sv3){.x = (a.y * b.z) - (a.z * b.y),
               .y = -((a.x * b.z) - (a.z * b.x)),
               .z = (a.x * b.y) - (a.y * b.x)};
}

// Returns the point that is t% of the way between a and b.
SV_API sv2 sv2_lerp(sv2 a, sv2 b, float t) {
  return (sv2){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
  };
}

// Returns the point that is t% of the way between a and b.
SV_API sv3 sv3_lerp(sv3 a, sv3 b, float t) {
  return (sv3){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
      .z = a.z + (b.z - a.z) * t,
  };
}

// Returns the point that is t% of the way between a and b.
SV_API sv4 sv4_lerp(sv4 a, sv4 b, float t) {
  return (sv4){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
      .z = a.z + (b.z - a.z) * t,
      .w = a.w + (b.w - a.w) * t,
  };
}

// Returns a vector with the smallest components from both a and b.
SV_API sv2 sv2_min(sv2 a, sv2 b) {
  return (sv2){
      .x = a.x <= b.x ? a.x : b.x,
      .y = a.y <= b.y ? a.y : b.y,
  };
}

// Returns a vector with the smallest components from both a and b.
SV_API sv3 sv3_min(sv3 a, sv3 b) {
  return (sv3){
      .x = a.x <= b.x ? a.x : b.x,
      .y = a.y <= b.y ? a.y : b.y,
      .z = a.z <= b.z ? a.z : b.z,
  };
}

// Returns a vector with the smallest components from both a and b.
SV_API sv4 sv4_min(sv4 a, sv4 b) {
  return (sv4){
      .x = a.x <= b.x ? a.x : b.x,
      .y = a.y <= b.y ? a.y : b.y,
      .z = a.z <= b.z ? a.z : b.z,
      .w = a.w <= b.w ? a.w : b.w,
  };
}

// Returns a vector with the biggest components from both a and b.
SV_API sv2 sv2_max(sv2 a, sv2 b) {
  return (sv2){
      .x = a.x >= b.x ? a.x : b.x,
      .y = a.y >= b.y ? a.y : b.y,
  };
}

// Returns a vector with the biggest components from both a and b.
SV_API sv3 sv3_max(sv3 a, sv3 b) {
  return (sv3){
      .x = a.x >= b.x ? a.x : b.x,
      .y = a.y >= b.y ? a.y : b.y,
      .z = a.z >= b.z ? a.z : b.z,
  };
}

// Returns a vector with the biggest components from both a and b.
SV_API sv4 sv4_max(sv4 a, sv4 b) {
  return (sv4){
      .x = a.x >= b.x ? a.x : b.x,
      .y = a.y >= b.y ? a.y : b.y,
      .z = a.z >= b.z ? a.z : b.z,
      .w = a.w >= b.w ? a.w : b.w,
  };
}

// Returns a seeded pseudo-random unit length vector
SV_API sv2 sv2_noise(sv2 seed) {
  sv2 v = (sv2){
      .x = sinf(seed.x * 53 + seed.y * 97) * 6151,
      .y = sinf(seed.x * 97 + seed.y * 53) * 193,
  };

  v.x -= floorf(v.x);
  v.y -= floorf(v.y);
  return v;
}

// Two dimensional pseudo-random noise
static inline float sv_noise2(unsigned int x, unsigned int y) {
  float wave = sinf(x * 53 + y * 97) * 6151;
  return wave - floorf(wave);
}

// Three dimensional pseudo-random noise
static inline float sv_noise3(unsigned int x, unsigned int y, unsigned int z) {
  float wave = sinf(x * 53 + y * 97 + z * 193) * 6151;
  return wave - floorf(wave);
}

#if 1
SV_API sv3 sv3_noise(unsigned int seed, float range) {
  float d, x, y, z;
  int i = 0;
  do {
    x = sv_noise3(i + seed + 1, i + seed, i + seed) * (2.0) - 1.0;
    y = sv_noise3(i + seed, i + seed + 1, i + seed) * (2.0) - 1.0;
    z = sv_noise3(i + seed, i + seed, i + seed + 1) * (2.0) - 1.0;
    d = x * x + y * y + z * z;
    i++;
  } while (d > 1.0);
  return sv3_scaled((sv3){x, y, z}, range);
}
#else
// Returns a seeded pseudo-random unit length vector
SV_API sv3 sv3_noise(sv3 seed) {
  sv3 v = {0};
  do {
    v = (sv3){
        .x = sinf(seed.x * 53 + seed.y * 97 + seed.z * 193) * 6151,
        .y = sinf(seed.x * 97 + seed.y * 53 + seed.z * 6151) * 193,
        .z = sinf(seed.x * 193 + seed.y * 6151 + seed.z * 97) * 53,
    };
  
    v.x -= floorf(v.x);
    v.y -= floorf(v.y);
    v.z -= floorf(v.z);
    
    sv3_give(&seed, (sv3){1,1,1});
    
  } while (sv3_magnitude(v) > 1.0);
  sv3_give(&v, (sv3){-1,-1,-1});
  return v;
}
#endif

// Returns a seeded pseudo-random unit length vector
SV_API sv4 sv4_noise(sv4 seed) {
  sv4 v = (sv4){
      .x = sinf(seed.x * 53 + seed.y * 97 + seed.z * 193) * 6151,
      .y = sinf(seed.x * 97 + seed.y * 53 + seed.z * 6151) * 193,
      .z = sinf(seed.x * 193 + seed.y * 6151 + seed.z * 97) * 53,
      .w = sinf(seed.x * 6151 + seed.y * 193 + seed.z * 53) * 97,
  };

  v.x -= floorf(v.x);
  v.y -= floorf(v.y);
  v.z -= floorf(v.z);
  return v;
}

// Returns true if a is equal to b.
SV_API int sv2_equal(sv2 a, sv2 b) {
  return fabs(a.x - b.x) <= SV__FLOAT_EPSILON &&
         fabs(a.y - b.y) <= SV__FLOAT_EPSILON;
}

// Returns true if a is equal to b.
SV_API int sv3_equal(sv3 a, sv3 b) {
  return fabs(a.x - b.x) <= SV__FLOAT_EPSILON &&
         fabs(a.y - b.y) <= SV__FLOAT_EPSILON &&
         fabs(a.z - b.z) <= SV__FLOAT_EPSILON;
}

// Returns true if a is equal to b.
SV_API int sv4_equal(sv4 a, sv4 b) {
  return fabs(a.x - b.x) <= SV__FLOAT_EPSILON &&
         fabs(a.y - b.y) <= SV__FLOAT_EPSILON &&
         fabs(a.z - b.z) <= SV__FLOAT_EPSILON &&
         fabs(a.w - b.w) <= SV__FLOAT_EPSILON;
}

// Multiply q1 with q2 (perform rotation on q1 by q2)
SV_API sv4 svq_multiply(sv4 q1, sv4 q2) {
  sv4 ret = {0};
  ret.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  ret.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
  ret.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
  ret.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
  return ret;
}

SV_API sv4 svq_conjugate(sv4 q) { return (sv4){-q.x, -q.y, -q.z, q.w}; }

// Rotates v by q
SV_API sv3 sv3_rotate(sv3 v, sv4 q) {
  sv4 ret = (sv4){v.x, v.y, v.z, 0.0};
  ret = svq_multiply(svq_multiply(q, ret), svq_conjugate(q));
  return (sv3){ret.x, ret.y, ret.z};
}

SV_API sv4 svq_from_euler(sv3 euler_angles) {
  sv4 q;

  float cos_roll = cosf(euler_angles.x * 0.5f);
  float sin_roll = sinf(euler_angles.x * 0.5f);
  float cos_pitch = cosf(euler_angles.y * 0.5f);
  float sin_pitch = sinf(euler_angles.y * 0.5f);
  float cos_yaw = cosf(euler_angles.z * 0.5f);
  float sin_yaw = sinf(euler_angles.z * 0.5f);
  q.w = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;
  q.x = sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw;
  q.y = cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw;
  q.z = cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw;

  return q;
}

SV_API sv4 svq_rotate_euler(sv4 q, sv3 euler_angles) {
  return svq_multiply(q, svq_from_euler(euler_angles));
}

SV_API float *svq_to_mat4(sv4 q, float *mat) {
  float xx = q.x * q.x;
  float xy = q.x * q.y;
  float xz = q.x * q.z;
  float xw = q.x * q.w;

  float yy = q.y * q.y;
  float yz = q.y * q.z;
  float yw = q.y * q.w;

  float zz = q.z * q.z;
  float zw = q.z * q.w;

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

#endif // SIMPLE_3D_H
