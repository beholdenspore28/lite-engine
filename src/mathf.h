#ifndef MATHF_H
#define MATHF_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MATHF_PI 3.14159265358
#define MATHF_API static inline

#if MATHF_DOUBLE_PRECISION
typedef double mathf_real;
#define MATHF_EPSILON (1e-8)
#define mathf_fmod fmod
#define mathf_fabs fabs
#define mathf_sin sin
#define mathf_cos cos
#define mathf_pow pow
#define mathf_floor floor
#else
typedef float mathf_real;
#define MATHF_EPSILON (1e-4)
#define mathf_fmod fmodf
#define mathf_fabs fabsf
#define mathf_sin sinf
#define mathf_cos cosf
#define mathf_pow powf
#define mathf_floor floorf
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

MATHF_API mathf_real mathf_rad2deg(const mathf_real n) { return n * (180.0 / MATHF_PI); }

MATHF_API mathf_real mathf_deg2rad(const mathf_real n) { return n * (MATHF_PI / 180.0); }

MATHF_API mathf_real mathf_max(mathf_real a, mathf_real b) {
  if (a > b) { return a; } else { return b; }
}

MATHF_API mathf_real mathf_min(mathf_real a, mathf_real b) {
  if (a < b) { return a; } else { return b; }
}

MATHF_API void mathf_swap(mathf_real *a, mathf_real *b) {
  mathf_real tmp = *a;
  *a = *b;
  *b = tmp;
}

MATHF_API mathf_real mathf_wrap_max(mathf_real n, mathf_real max) {
  return mathf_fmod(max + mathf_fmod(n, max), max);
}

MATHF_API mathf_real mathf_wrap(mathf_real n, mathf_real min, mathf_real max) {
  return min + mathf_wrap_max(n - min, max - min);
}

MATHF_API mathf_real mathf_clamp(mathf_real n, const mathf_real min, const mathf_real max) {
  n = n < min ? min : n;
  return n > max ? max : n;
}

MATHF_API mathf_real mathf_clamp01(mathf_real n) {
  n = n < 0 ? 0 : n;
  return n > 1 ? 1 : n;
}

MATHF_API mathf_real mathf_lerp(mathf_real a, mathf_real b, mathf_real t) { return a + (b - a) * t; }

MATHF_API mathf_real mathf_lerpclamped(mathf_real a, mathf_real b, mathf_real t) {
  return a + (b - a) * mathf_clamp01(t);
}

MATHF_API mathf_real mathf_norm(mathf_real n, mathf_real min, mathf_real max) {
  return (n - min) / (max - min);
}

MATHF_API mathf_real mathf_map(mathf_real n, mathf_real fromMin, mathf_real fromMax, mathf_real toMin,
                        mathf_real toMax) {
  // return lerp(norm(n, fromMin, fromMax), toMin, toMax);
  return (n - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin;
}

MATHF_API int mathf_aproxequal(mathf_real a, mathf_real b, mathf_real tolerance) {
  return (mathf_fabs(a - b) < tolerance);
}

MATHF_API mathf_real mathf_cosInterpolate(mathf_real a, mathf_real b, mathf_real t) {
  mathf_real f = (1.0f - mathf_cos(t * MATHF_PI)) * 0.5f;
  return a * (1.0 - f) + b * f;
}

MATHF_API mathf_real mathf_sigmoid(mathf_real n) {
  return (1 / (1 + mathf_pow(2.71828182846, -n)));
}

MATHF_API mathf_real mathf_loop(mathf_real n, const mathf_real length) {
  return mathf_clamp(n - mathf_floor(n / length) * length, 0.0f, length);
}

MATHF_API mathf_real mathf_ping_pong(mathf_real n, const mathf_real length) {
  n = mathf_loop(n, length * 2.0f);
  return mathf_fabs(n - length);
}

MATHF_API mathf_real mathf_angleDelta(const mathf_real a, const mathf_real b) {
  mathf_real delta = mathf_loop((b - a), 360.0f);
  if (delta > 180.0f) {
    delta -= 360.0f;
  }
  return delta;
}

MATHF_API mathf_real mathf_fraction(mathf_real x) { return x - mathf_floor(x); }

// Single dimensional pseudo-random noise
MATHF_API mathf_real mathf_noise1(unsigned int x) {
  mathf_real wave = mathf_sin(x * 53) * 6151;
  return mathf_fraction(wave);
}

// Two dimensional pseudo-random noise
MATHF_API mathf_real mathf_noise2(unsigned int x, unsigned int y) {
  mathf_real wave = mathf_sin(x * 53 + y * 97) * 6151;
  return mathf_fraction(wave);
}

// Three dimensional pseudo-random noise
MATHF_API mathf_real mathf_noise3(unsigned int x, unsigned int y, unsigned int z) {
  mathf_real wave = mathf_sin(x * 53 + y * 97 + z * 193) * 6151;
  return mathf_fraction(wave);
}

// Three dimensional pseudo-random noise
MATHF_API mathf_real mathf_noise3_interpolated(mathf_real x, mathf_real y, mathf_real z) {
  mathf_real fractX = mathf_fraction(x), fractY = mathf_fraction(y), fractZ = mathf_fraction(z),
        floorX = mathf_floor(x), floorY = mathf_floor(y), floorZ = mathf_floor(z);

  // interpolate between adjacent noise values
  // ==================================================
  // two vertices 'v' make an edge 'e'
  // two edges make a face 'f'
  // two faces make a cube.
  // ==================================================

  //===================================================
  mathf_real v1 = mathf_noise3(floorX, floorY, floorZ),
        v2 = mathf_noise3(floorX + 1, floorY, floorZ),
        e1 = mathf_cosInterpolate(v1, v2, fractX), // rear bottom

      v3 = mathf_noise3(floorX, floorY + 1, floorZ),
        v4 = mathf_noise3(floorX + 1, floorY + 1, floorZ),
        e2 = mathf_cosInterpolate(v3, v4, fractX), // rear top

      v5 = mathf_noise3(floorX, floorY, floorZ + 1),
        v6 = mathf_noise3(floorX + 1, floorY, floorZ + 1),
        e3 = mathf_cosInterpolate(v5, v6, fractX), // front bottom

      v7 = mathf_noise3(floorX, floorY + 1, floorZ + 1),
        v8 = mathf_noise3(floorX + 1, floorY + 1, floorZ + 1),
        e4 = mathf_cosInterpolate(v7, v8, fractX), // front top

      f1 = mathf_cosInterpolate(e1, e2, fractY), f2 = mathf_cosInterpolate(e3, e4, fractY),
        cube = mathf_cosInterpolate(f1, f2, fractZ);

  return cube;
}

MATHF_API mathf_real mathf_noise3_fbm(mathf_real x, mathf_real y, mathf_real z) {
  mathf_real total = 0.0;
  mathf_real freq = 1.0;
  mathf_real amplitude = 1.0;
  mathf_real persistance = 0.5;
  int octaves = 16;
  for (int i = 0; i < octaves; i++) {
    freq = mathf_pow(2, i);
    amplitude = mathf_pow(persistance, i);
    total += mathf_noise3_interpolated(x * freq, y * freq, z * freq) * amplitude;
  }
  return total;
}

MATHF_API mathf_real mathf_noise3_fbm_warped(mathf_real x, mathf_real y, mathf_real z,
                                      mathf_real warpFactor) {
  mathf_real fbm1 = mathf_noise3_fbm(x, y, z);
  mathf_real fbm2 = mathf_noise3_fbm(x + 5.2, y + 1.3 * warpFactor, z + 6.4 * warpFactor);
  mathf_real fbm3 = mathf_noise3_fbm(x + 7.5, y + 0.3 * warpFactor, z + 3.6 * warpFactor);
  return mathf_noise3_fbm(fbm1, fbm2, fbm3);
}

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

#endif // MATHF_H
