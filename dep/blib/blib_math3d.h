/*----------------------------------LEGAL--------------------------------------

MIT License

Copyright (c) 2023 Benjamin Joseph Brooks

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-----------------------------------------------------------------------------*/
#ifndef BLIB_MATH3D_H
#define BLIB_MATH3D_H

#include "blib/blib.h"
#include "blib_math.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
  float x;
  float y;
} vector2;
DECLARE_LIST(vector2)

typedef struct {
  float x;
  float y;
  float z;
} vector3;
DECLARE_LIST(vector3)

typedef struct {
  float x;
  float y;
  float z;
  float w;
} vector4;
DECLARE_LIST(vector4)

typedef struct {
  float w;
  float x;
  float y;
  float z;
} quaternion;
DECLARE_LIST(quaternion)

static inline vector2 vector2_zero(void) { return (vector2){0.0f, 0.0f}; }
static inline vector2 vector2_one(float s) { return (vector2){s, s}; }
static inline vector2 vector2_up(float s) { return (vector2){0.0f, s}; }
static inline vector2 vector2_down(float s) { return (vector2){0.0f, -s}; }
static inline vector2 vector2_right(float s) { return (vector2){s, 0.0f}; }
static inline vector2 vector2_left(float s) { return (vector2){-s, 0.0f}; }

static inline vector3 vector3_zero(void) { return (vector3){0.0f, 0.0f, 0.0f}; }
static inline vector3 vector3_one(float s) { return (vector3){s, s, s}; }
static inline vector3 vector3_up(float s) { return (vector3){0.0f, s, 0.0f}; }
static inline vector3 vector3_down(float s) {
  return (vector3){0.0f, -s, 0.0f};
}
static inline vector3 vector3_right(float s) {
  return (vector3){s, 0.0f, 0.0f};
}
static inline vector3 vector3_left(float s) {
  return (vector3){-s, 0.0f, 0.0f};
}
static inline vector3 vector3_forward(float s) {
  return (vector3){0.0f, 0.0f, s};
}
static inline vector3 vector3_back(float s) {
  return (vector3){0.0f, 0.0f, -s};
}

static inline vector4 vector4_zero(void) {
  return (vector4){0.0f, 0.0f, 0.0f, 1.0f};
}
static inline vector4 vector4_one(float s) { return (vector4){s, s, s, 1.0f}; }
static inline vector4 vector4_up(float s) {
  return (vector4){0.0f, s, 0.0f, 1.0f};
}
static inline vector4 vector4_down(float s) {
  return (vector4){0.0f, -s, 0.0f, 1.0f};
}
static inline vector4 vector4_right(float s) {
  return (vector4){s, 0.0f, 0.0f, 1.0f};
}
static inline vector4 vector4_left(float s) {
  return (vector4){-s, 0.0f, 0.0f, 1.0f};
}
static inline vector4 vector4_forward(float s) {
  return (vector4){0.0f, 0.0f, s, 1.0f};
}
static inline vector4 vector4_back(float s) {
  return (vector4){0.0f, 0.0f, -s, 1.0f};
}

/*Prints a vec "v" using printf*/
static inline void vector2_print(const vector2 v, const char *label) {
  printf("%s [%f, %f]\n", label, v.x, v.y);
}

/*Returns a vec that points in the opposite direction of the given vec
  "v". The vec returned has a magnitude identical to the given vec "v"*/
static inline vector2 vector2_negate(vector2 v) {
  return (vector2){.x = -v.x, .y = -v.y};
}

/*Subtracts a vec "subtrahend" from another vec "minuend"*/
static inline vector2 vector2_subtract(vector2 minuend, vector2 subtrahend) {
  return (vector2){.x = minuend.x - subtrahend.x,
                   .y = minuend.y - subtrahend.y};
}

/*Adds a vec "a" to another vec "b"*/
static inline vector2 vector2_add(vector2 a, vector2 b) {
  return (vector2){.x = a.x + b.x, .y = a.y + b.y};
}

/*A more performant way of getting the relative length of a
  vec "v". This saves a square root operation making it more
  performant than vector2_Magnitude(). If all you have to do is
  compare a vecs length relatively, use this function instead of
  vector2_Magnitude()*/
static inline float vector2_square_magnitude(vector2 v) {
  return ((v.x * v.x) + (v.y * v.y));
}

/*Returns the actual length of a vec "v".
  This uses a square root operation. Use vector2_SquareMagnitudenitude()
  to sacrifice accuracy and save on performance when comparing
  distances.*/
static inline float vector2_magnitude(vector2 v) {
  return sqrt(vector2_square_magnitude(v));
}

/*Returns a given vec "v" as a unit vec.
  This means the magnitude(length) of the returned
  vec will always be 1 unit. The returned vec always points
  in the same direction as the given vec "v"*/
static inline vector2 vector2_normalize(vector2 v) {
  float m = vector2_magnitude(v);
  if (m == 0)
    return vector2_zero();
  return (vector2){.x = v.x / m, .y = v.y / m};
}

/*Returns the distance between point a and point b
  in units.*/
static inline float vector2_distance(vector2 a, vector2 b) {
  return vector2_magnitude(vector2_subtract(b, a));
}

/*Returns the distance squared between point a and point b
  in units. Faster than vector2_distance, but the result is the distance
  squared*/
static inline float vector2_square_distance(vector2 a, vector2 b) {
  return vector2_square_magnitude(vector2_subtract(b, a));
}

/*Scales a vec "v" by "scalar".
  increases the magnitude when "scalar" is greater than 1.
  decreases the magnitude when "scalar" is less than 0.
  The returned vec will point in the same direction as
  the given vec "v".*/
static inline vector2 vector2_scale(vector2 v, float scalar) {
  return (vector2){.x = v.x * scalar, .y = v.y * scalar};
}

/*For normalized vecs Dot returns 1 if they point in
  exactly the same direction, -1 if they point in completely opposite directions
  and zero if the vecs are perpendicular.*/
static inline float vector2_dot(vector2 a, vector2 b) {
  return (a.x * b.x) + (a.y * b.y);
}

/* Returns the position of a physical body over time by using the given
 * 'acceleration' 'velocity' 'position' and 'time' arguments*/
static inline vector3 vector3_kinematic_equation(vector3 acceleration,
                                                 vector3 velocity,
                                                 vector3 position, float time) {
  float x =
      0.5f * acceleration.x * time * time + velocity.x * time + position.x;
  float y =
      0.5f * acceleration.y * time * time + velocity.y * time + position.y;
  float z =
      0.5f * acceleration.z * time * time + velocity.z * time + position.z;
  return (vector3){x, y, z};
}

/*Linearly interpolates between "a" and "b" by "t".
If you want to make sure the returned value stays
between "a" and "b", use vector3_Lerpclamped() instead.
Returns a point at "t"% of the way between "a" and "b".*/
static inline vector2 vector2_lerp(vector2 a, vector2 b, float t) {
  return (vector2){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
  };
}

/*Linearly interpolates between "a" and "b" by "t".
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector2 vector2_lerp_clamped(vector2 a, vector2 b, float t) {
  /*clamp n between 0 and 1*/
  t = t < 0.0f ? 0.0f : t;
  t = t > 1.0f ? 1.0f : t;
  /*perform lerp*/
  return (vector2){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
  };
}

/*Converts a 2 dimensional vec to a 3 dimensional one*/
static inline vector3 vector2o_vector3(vector2 v) {
  return (vector3){.x = v.x, .y = v.y, .z = 0.0f};
}

/*Converts a 2 dimensional vec to a 4 dimensional one*/
static inline vector4 vector2o_vector4(vector2 v) {
  return (vector4){.x = v.x, .y = v.y, .z = 0.0f, .w = 1.0f};
}

/*Prints a vec "v" using printf*/
static inline void vector3_print(const vector3 v, const char *label) {
  printf("%s [%f, %f, %f]\n", label, v.x, v.y, v.z);
}

/*Returns a vec that points in the opposite direction of the given vec
  "v". The vec returned has a magnitude identical to the given vec "v"*/
static inline vector3 vector3_negate(vector3 v) {
  return (vector3){.x = -v.x, .y = -v.y, .z = -v.z};
}

/*Adds a vec "a" to another vec "b"*/
static inline vector3 vector3_add(vector3 a, vector3 b) {
  return (vector3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

/*Subtracts a vec "subtrahend" from another vec "minuend"*/
static inline vector3 vector3_subtract(vector3 minuend, vector3 subtrahend) {
  return (vector3){.x = minuend.x - subtrahend.x,
                   .y = minuend.y - subtrahend.y,
                   .z = minuend.z - subtrahend.z};
}

/*A more performant way of getting the relative length of a
  vec "v". This saves a square root operation making it more
  performant than vector3_Magnitude(). If all you have to do is
  compare a vecs length relatively, use this function instead of
  vector3_Magnitude()*/
static inline float vector3_square_magnitude(vector3 v) {
  return ((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/*Returns the actual length of a vec "v".
  This uses a square root operation. Use vector3_SquareMagnitudenitude()
  to sacrifice accuracy and save on performance when comparing
  distances.*/
static inline float vector3_magnitude(vector3 v) {
  return sqrt(vector3_square_magnitude(v));
}

/*Returns a given vec "v" as a unit vec.
  This means the magnitude(length) of the returned
  vec will always be 1 unit. The returned vec always points
  in the same direction as the given vec "v"*/
static inline vector3 vector3_normalize(vector3 v) {
  float m = vector3_magnitude(v);
  if (m == 0)
    return vector3_zero();
  return (vector3){.x = v.x / m, .y = v.y / m, .z = v.z / m};
}

/*Returns the distance between point a and point b
  in units.*/
static inline float vector3_distance(vector3 a, vector3 b) {
  return vector3_magnitude(vector3_subtract(b, a));
}

/*Returns the distance squared between point a and point b
  in units. Faster than vector3_distance, but the result is the distance
  squared*/
static inline float vector3_square_distance(vector3 a, vector3 b) {
  return vector3_square_magnitude(vector3_subtract(b, a));
}

/*Scales a vec "v" by "scalar".
  increases the magnitude when "scalar" is greater than 1.
  decreases the magnitude when "scalar" is less than 1.
  The returned vec will point in the same direction as
  the given vec "v".*/
static inline vector3 vector3_scale(vector3 v, float scalar) {
  return (vector3){.x = v.x * scalar, .y = v.y * scalar, .z = v.z * scalar};
}

/*Returns a vec parallel to both "a" and "b".*/
static inline vector3 vector3_cross(vector3 a, vector3 b) {
  return (vector3){.x = (a.y * b.z) - (a.z * b.y),
                   .y = -((a.x * b.z) - (a.z * b.x)),
                   .z = (a.x * b.y) - (a.y * b.x)};
}

/*For normalized vecs Dot returns 1 if they point in
  exactly the same direction, -1 if they point in completely opposite directions
  and zero if the vecs are perpendicular.*/
static inline float vector3_dot(vector3 a, vector3 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

/*Linearly interpolates between "a" and "b" by "t".
  If you want to make sure the returned value stays
  between "a" and "b", use vector2_Lerpclamped() instead.
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector3 vector3_lerp(vector3 a, vector3 b, float t) {
  return (vector3){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
      .z = a.z + (b.z - a.z) * t,
  };
}

/*Linearly interpolates between "a" and "b" by "t".
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector3 vector3_lerp_clamped(vector3 a, vector3 b, float n) {
  /*clamp n between 0 and 1*/
  n = n < 0.0f ? 0.0f : n;
  n = n > 1.0f ? 1.0f : n;
  /*perform lerp*/
  return (vector3){
      .x = a.x + (b.x - a.x) * n,
      .y = a.y + (b.y - a.y) * n,
      .z = a.z + (b.z - a.z) * n,
  };
}

/*Returns a vec that is made from the largest components of two
  vecs.*/
static inline vector3 vector3_max(vector3 a, vector3 b) {
  return (vector3){
      .x = a.x >= b.x ? a.x : b.x,
      .y = a.y >= b.y ? a.y : b.y,
      .z = a.z >= b.z ? a.z : b.z,
  };
}

/*Returns a vec that is made from the smallest components of two
  vecs.*/
static inline vector3 vector3_min(vector3 a, vector3 b) {
  return (vector3){
      .x = a.x <= b.x ? a.x : b.x,
      .y = a.y <= b.y ? a.y : b.y,
      .z = a.z <= b.z ? a.z : b.z,
  };
}

/*Converts a 3 dimensional vec to a 2 dimensional one*/
static inline vector2 vector3o_vector2(vector3 v) {
  return (vector2){.x = v.x, .y = v.y};
}

/*Converts a 3 dimensional vec to a 4 dimensional one*/
static inline vector4 vector3o_vector4(vector3 v) {
  return (vector4){.x = v.x, .y = v.y, .z = v.z, .w = 1.0f};
}

// returns a random point from inside 'bounds'
static inline vector3 vector3_random_box(unsigned int seed, vector3 bounds) {
  vector3 ret = vector3_zero();
  ret.x = noise3(seed + 1, seed, seed) * (2 * bounds.x) - bounds.x;
  ret.y = noise3(seed, seed + 1, seed) * (2 * bounds.y) - bounds.y;
  ret.z = noise3(seed, seed, seed + 1) * (2 * bounds.z) - bounds.z;
  return ret;
}

// returns a random 'range' length vector
static inline vector3 vector3_random(unsigned int seed, float range) {
  float d, x, y, z;
  int i = 0;
  do {
    x = noise3(i + seed + 1, i + seed, i + seed) * (2.0 * range) - range;
    y = noise3(i + seed, i + seed + 1, i + seed) * (2.0 * range) - range;
    z = noise3(i + seed, i + seed, i + seed + 1) * (2.0 * range) - range;
    d = x * x + y * y + z * z;
    i++;
  } while (d > range);
  return (vector3){x, y, z};
}

/*Prints a vec "v" using printf*/
static inline void vector4_print(const vector4 v, const char *label) {
  printf("%s [%f, %f, %f, %f]\n", label, v.x, v.y, v.z, v.w);
}

/*Returns a vec that points in the opposite direction of the given vec
  "v". The vec returned has a magnitude identical to the given vec "v"*/
static inline vector4 vector4_negate(vector4 v) {
  return (vector4){.x = -v.x, .y = -v.y, .z = -v.z, .w = -v.w};
}

/*A more performant way of getting the relative length of a
  vec "v". This saves a square root operation making it more
  performant than vector4_Magnitude(). If all you have to do is
  compare a vecs length relatively, use this function instead of
  vector4_Magnitude()*/
static inline float vector4_square_magnitude(vector4 v) {
  return ((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}

/*Returns the actual length of a vec "v".
  This uses a square root operation. Use vector4_SquareMagnitudenitude()
  to sacrifice accuracy and save on performance when comparing
  distances.*/
static inline float vector4_magnitude(vector4 v) {
  return sqrt(vector4_square_magnitude(v));
}

/*Returns a given vec "v" as a unit vec.
  This means the magnitude(length) of the returned
  vec will always be 1 unit. The returned vec always points
  in the same direction as the given vec "v"*/
static inline vector4 vector4_normalize(vector4 v) {
  float m = vector4_magnitude(v);
  if (m == 0)
    return vector4_zero();
  return (vector4){.x = v.x / m, .y = v.y / m, .z = v.z / m, .w = v.w / m};
}

/*Adds a vec "a" to another vec "b"*/
static inline vector4 vector4_add(vector4 a, vector4 b) {
  return (vector4){
      .x = a.x + b.x,
      .y = a.y + b.y,
      .z = a.z + b.z,
      .w = a.w + b.w,
  };
}

/*Subtracts a vec "subtrahend" from another vec "minuend"*/
static inline vector4 vector4_subtract(vector4 minuend, vector4 subtrahend) {
  return (vector4){
      .x = minuend.x - subtrahend.x,
      .y = minuend.y - subtrahend.y,
      .z = minuend.z - subtrahend.z,
      .w = minuend.w - subtrahend.w,
  };
}

/*Returns the distance between point a and point b
  in units.*/
static inline float vector4_distance(vector4 a, vector4 b) {
  return vector4_magnitude(vector4_subtract(b, a));
}

/*Returns the distance squared between point a and point b
  in units. Faster than vector4_distance, but the result is the distance
  squared*/
static inline float vector4_square_distance(vector4 a, vector4 b) {
  return vector4_square_magnitude(vector4_subtract(b, a));
}

/*Scales a vec "v" by "scalar".
  increases the magnitude when "scalar" is greater than 1.
  decreases the magnitude when "scalar" is less than 1.
  The returned vec will point in the same direction as
  the given vec "v".*/
static inline vector4 vector4_scale(vector4 v, float scalar) {
  return (vector4){
      .x = v.x * scalar,
      .y = v.y * scalar,
      .z = v.z * scalar,
      .w = v.w * scalar,
  };
}

/*For normalized vecs Dot returns 1 if they point in
  exactly the same direction, -1 if they point in completely opposite directions
  and zero if the vecs are perpendicular.*/
static inline float vector4_dot(vector4 a, vector4 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

/*Linearly interpolates between "a" and "b" by "t".
  If you want to make sure the returned value stays
  between "a" and "b", use vector2_lerpclamped() instead.
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector4 vector4_lerp(vector4 a, vector4 b, float t) {
  return (vector4){
      .x = a.x + (b.x - a.x) * t,
      .y = a.y + (b.y - a.y) * t,
      .z = a.z + (b.z - a.z) * t,
      .w = a.w + (b.w - a.w) * t,
  };
}

/*Linearly interpolates between "a" and "b" by "t".
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector4 vector4_Lerpclamped(vector4 a, vector4 b, float n) {
  /*clamp n between 0 and 1*/
  n = n < 0.0f ? 0.0f : n;
  n = n > 1.0f ? 1.0f : n;
  /*perform lerp*/
  return (vector4){
      .x = a.x + (b.x - a.x) * n,
      .y = a.y + (b.y - a.y) * n,
      .z = a.z + (b.z - a.z) * n,
      .w = a.w + (b.w - a.w) * n,
  };
}

/*Returns a vec that is made from the largest components of two
  vecs.*/
static inline vector4 vector4_Max(vector4 a, vector4 b) {
  return (vector4){
      .x = a.x >= b.x ? a.x : b.x,
      .y = a.y >= b.y ? a.y : b.y,
      .z = a.z >= b.z ? a.z : b.z,
      .w = a.w >= b.w ? a.w : b.w,
  };
}

/*Returns a vec that is made from the smallest components of two
  vecs.*/
static inline vector4 vector4_Min(vector4 a, vector4 b) {
  return (vector4){
      .x = a.x <= b.x ? a.x : b.x,
      .y = a.y <= b.y ? a.y : b.y,
      .z = a.z <= b.z ? a.z : b.z,
      .w = a.w <= b.w ? a.w : b.w,
  };
}

/*Converts a 4 dimensional vec to a 2 dimensional one*/
static inline vector2 vector4ovector2(vector4 v) {
  return (vector2){.x = v.x, .y = v.y};
}

/*Converts a 4 dimensional vec to a 3 dimensional one*/
static inline vector3 vector4ovector3(vector4 v) {
  return (vector3){.x = v.x, .y = v.y, .z = v.z};
}

static inline quaternion quaternion_identity(void) {
  return (quaternion){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
}

static inline int quaternion_equal(quaternion a, quaternion b) {
  return fabs(a.x - b.x) <= FLOAT_EPSILON && fabs(a.y - b.y) <= FLOAT_EPSILON &&
         fabs(a.z - b.z) <= FLOAT_EPSILON && fabs(a.w - b.w) <= FLOAT_EPSILON;
}

static inline quaternion quaternion_from_angle_axis(float angle, vector3 axis) {
  quaternion ret;
  float s = sinf(angle / 2);
  ret.x = axis.x * s;
  ret.y = axis.y * s;
  ret.z = axis.z * s;
  ret.w = cosf(angle / 2);
  return ret;
}

static inline float quaternion_magnitude(quaternion q) {
  return sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

static inline quaternion quaternion_normalize(quaternion q) {
  float mag = quaternion_magnitude(q);
  if (mag == 0) {
    return quaternion_identity();
  }
  q.w /= mag;
  q.x /= mag;
  q.y /= mag;
  q.z /= mag;
  return q;
}

static inline quaternion quaternion_conjugate(quaternion q) {
  return (quaternion){.w = q.w, .x = -q.x, .y = -q.y, .z = -q.z};
}

static inline quaternion quaternion_inverse(quaternion q) {
  return (quaternion){
      .w = -q.w,
      .x = -q.x,
      .y = -q.y,
      .z = -q.z,
  };
}

static inline void quaternion_print(quaternion q, const char *label) {
  printf("\t%12f, %12f, %12f, %12f\t%s\n", q.x, q.y, q.z, q.w, label);
}

static inline quaternion quaternion_add(quaternion q1, quaternion q2) {
  return (quaternion){
      .w = q1.w + q2.w,
      .x = q1.x + q2.x,
      .y = q1.y + q2.y,
      .z = q1.z + q2.z,
  };
}

static inline quaternion quaternion_multiply(quaternion q1, quaternion q2) {
  quaternion ret = {0};
  ret.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  ret.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
  ret.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
  ret.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
  return ret;
}

static inline quaternion quaternion_scale(quaternion q, float scalar) {
  q.x *= scalar;
  q.y *= scalar;
  q.z *= scalar;
  q.w *= scalar;
  return q;
}

/*
   Returns the given vec 'v' rotated by the quaternion 'rotation'.
   */
static inline vector3 vector3_rotate(vector3 v, quaternion rotation) {
  quaternion ret = (quaternion){
      .w = 0,
      .x = v.x,
      .y = v.y,
      .z = v.z,
  };

  ret = quaternion_multiply(quaternion_multiply(rotation, ret),
                            quaternion_conjugate(rotation));

  return (vector3){
      .x = ret.x,
      .y = ret.y,
      .z = ret.z,
  };
}

static inline quaternion quaternion_from_euler(vector3 eulerAngles) {
  quaternion q;

  float cRoll = cosf(eulerAngles.x * 0.5f);
  float sRoll = sinf(eulerAngles.x * 0.5f);
  float cPitch = cosf(eulerAngles.y * 0.5f);
  float sPitch = sinf(eulerAngles.y * 0.5f);
  float cYaw = cosf(eulerAngles.z * 0.5f);
  float sYaw = sinf(eulerAngles.z * 0.5f);
  q.w = cRoll * cPitch * cYaw + sRoll * sPitch * sYaw;
  q.x = sRoll * cPitch * cYaw - cRoll * sPitch * sYaw;
  q.y = cRoll * sPitch * cYaw + sRoll * cPitch * sYaw;
  q.z = cRoll * cPitch * sYaw - sRoll * sPitch * cYaw;

  return q;
}

static inline quaternion quaternion_rotate_euler(quaternion q,
                                                 vector3 euler_angles) {
  return quaternion_multiply(q, quaternion_from_euler(euler_angles));
}

static inline float *quaternion_to_mat4(quaternion q, float *mat) {
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

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

#endif // BLIB_MATH3D_H

#ifdef BLIB_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

DEFINE_LIST(vector4)
DEFINE_LIST(vector3)
DEFINE_LIST(vector2)
DEFINE_LIST(quaternion)
// DEFINE_LIST(matrix4_t)

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

#endif
