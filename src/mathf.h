#ifndef MATHF_H
#define MATHF_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MATHF_API static inline
#define MATHF_PI 3.14159265358
#define MATHF_EPSILON (1e-4)
#define mathf_fmod fmodf
#define mathf_fabs fabsf
#define mathf_sin sinf
#define mathf_cos cosf
#define mathf_pow powf
#define mathf_floor floorf

MATHF_API float mathf_rad2deg(const float n) { return n * (180.0 / MATHF_PI); }

MATHF_API float mathf_deg2rad(const float n) { return n * (MATHF_PI / 180.0); }

MATHF_API float mathf_max(float a, float b) {
  if (a > b) { return a; } else { return b; }
}

MATHF_API float mathf_min(float a, float b) {
  if (a < b) { return a; } else { return b; }
}

MATHF_API void mathf_swap(float *a, float *b) {
  float tmp = *a;
  *a = *b;
  *b = tmp;
}

MATHF_API float mathf_wrap_max(float n, float max) {
  return mathf_fmod(max + mathf_fmod(n, max), max);
}

MATHF_API float mathf_wrap(float n, float min, float max) {
  return min + mathf_wrap_max(n - min, max - min);
}

MATHF_API float mathf_clamp(float n, const float min, const float max) {
  n = n < min ? min : n;
  return n > max ? max : n;
}

MATHF_API float mathf_clamp01(float n) {
  n = n < 0 ? 0 : n;
  return n > 1 ? 1 : n;
}

MATHF_API float mathf_lerp(float a, float b, float t) { return a + (b - a) * t; }

MATHF_API float mathf_lerpclamped(float a, float b, float t) {
  return a + (b - a) * mathf_clamp01(t);
}

MATHF_API float mathf_norm(float n, float min, float max) {
  return (n - min) / (max - min);
}

MATHF_API float mathf_map(float n, float fromMin, float fromMax, float toMin,
                        float toMax) {
  // return lerp(norm(n, fromMin, fromMax), toMin, toMax);
  return (n - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin;
}

MATHF_API int mathf_equal(float a, float b, float tolerance) {
  return (mathf_fabs(a - b) < tolerance);
}

// cosine interpolation
MATHF_API float mathf_cerp(float a, float b, float t) {
  float f = (1.0 - mathf_cos(t * MATHF_PI)) * 0.5;
  return a * (1.0 - f) + b * f;
}

MATHF_API float mathf_loop(float n, const float length) {
  return mathf_clamp(n - mathf_floor(n / length) * length, 0.0, length);
}

MATHF_API float mathf_ping_pong(float n, const float length) {
  n = mathf_loop(n, length * 2.0);
  return mathf_fabs(n - length);
}

MATHF_API float mathf_angle_delta(const float a, const float b) {
  float delta = mathf_loop((b - a), 360.0);
  if (delta > 180.0) {
    delta -= 360.0;
  }
  return delta;
}

MATHF_API float mathf_fraction(float x) { return x - mathf_floor(x); }

// Single dimensional pseudo-random noise
MATHF_API float mathf_noise1(unsigned int x) {
  float wave = mathf_sin(x * 53) * 6151;
  return mathf_fraction(wave);
}

// Two dimensional pseudo-random noise
MATHF_API float mathf_noise2(unsigned int x, unsigned int y) {
  float wave = mathf_sin(x * 53 + y * 97) * 6151;
  return mathf_fraction(wave);
}

// Three dimensional pseudo-random noise
MATHF_API float mathf_noise3(unsigned int x, unsigned int y, unsigned int z) {
  float wave = mathf_sin(x * 53 + y * 97 + z * 193) * 6151;
  return mathf_fraction(wave);
}

// Three dimensional pseudo-random noise
MATHF_API float mathf_noise3_interpolated(float x, float y, float z) {
  float fractX = mathf_fraction(x), fractY = mathf_fraction(y), fractZ = mathf_fraction(z),
        floorX = mathf_floor(x), floorY = mathf_floor(y), floorZ = mathf_floor(z);

  // interpolate between adjacent noise values
  // ==================================================
  // two vertices 'v' make an edge 'e'
  // two edges make a face 'f'
  // two faces make a cube.
  // ==================================================

  //===================================================
  float v1 = mathf_noise3(floorX, floorY, floorZ),
        v2 = mathf_noise3(floorX + 1, floorY, floorZ),
        e1 = mathf_cerp(v1, v2, fractX), // rear bottom

      v3 = mathf_noise3(floorX, floorY + 1, floorZ),
        v4 = mathf_noise3(floorX + 1, floorY + 1, floorZ),
        e2 = mathf_cerp(v3, v4, fractX), // rear top

      v5 = mathf_noise3(floorX, floorY, floorZ + 1),
        v6 = mathf_noise3(floorX + 1, floorY, floorZ + 1),
        e3 = mathf_cerp(v5, v6, fractX), // front bottom

      v7 = mathf_noise3(floorX, floorY + 1, floorZ + 1),
        v8 = mathf_noise3(floorX + 1, floorY + 1, floorZ + 1),
        e4 = mathf_cerp(v7, v8, fractX), // front top

      f1 = mathf_cerp(e1, e2, fractY), f2 = mathf_cerp(e3, e4, fractY),
        cube = mathf_cerp(f1, f2, fractZ);

  return cube;
}

MATHF_API float mathf_noise3_fbm(float x, float y, float z) {
  float total = 0.0;
  float freq = 1.0;
  float amplitude = 1.0;
  float persistance = 0.5;
  int octaves = 16;
  for (int i = 0; i < octaves; i++) {
    freq = mathf_pow(2, i);
    amplitude = mathf_pow(persistance, i);
    total += mathf_noise3_interpolated(x * freq, y * freq, z * freq) * amplitude;
  }
  return total;
}

MATHF_API float mathf_noise3_fbm_warped(float x, float y, float z,
                                      float warpFactor) {
  float fbm1 = mathf_noise3_fbm(x, y, z);
  float fbm2 = mathf_noise3_fbm(x + 5.2, y + 1.3 * warpFactor, z + 6.4 * warpFactor);
  float fbm3 = mathf_noise3_fbm(x + 7.5, y + 0.3 * warpFactor, z + 3.6 * warpFactor);
  return mathf_noise3_fbm(fbm1, fbm2, fbm3);
}

#endif // MATHF_H
