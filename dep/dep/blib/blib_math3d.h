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
} vector2_t;
DECLARE_LIST(vector2_t)

typedef struct {
	float x;
	float y;
	float z;
} vector3_t;
DECLARE_LIST(vector3_t)

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vector4_t;
DECLARE_LIST(vector4_t)

typedef struct {
	float elements[16];
} matrix4_t;
DECLARE_LIST(matrix4_t)

typedef struct {
	float w;
	float x;
	float y;
	float z;
} quaternion_t;
DECLARE_LIST(quaternion_t)

static inline vector2_t vector2_zero     (void)    { return (vector2_t){ 0.0f,  0.0f }; }
static inline vector2_t vector2_one      (float s) { return (vector2_t){ s,     s    }; }
static inline vector2_t vector2_up       (float s) { return (vector2_t){ 0.0f,  s    }; }
static inline vector2_t vector2_down     (float s) { return (vector2_t){ 0.0f, -s    }; }
static inline vector2_t vector2_right    (float s) { return (vector2_t){ s,     0.0f }; }
static inline vector2_t vector2_left     (float s) { return (vector2_t){-s,     0.0f }; }

static inline vector3_t vector3_zero     (void)    { return (vector3_t){ 0.0f,  0.0f, 0.0f  }; }
static inline vector3_t vector3_one      (float s) { return (vector3_t){ s,     s,     s    }; }
static inline vector3_t vector3_up       (float s) { return (vector3_t){ 0.0f,  s,     0.0f }; }
static inline vector3_t vector3_down     (float s) { return (vector3_t){ 0.0f, -s,     0.0f }; }
static inline vector3_t vector3_right    (float s) { return (vector3_t){ s,     0.0f,  0.0f }; }
static inline vector3_t vector3_left     (float s) { return (vector3_t){-s,     0.0f,  0.0f }; }
static inline vector3_t vector3_forward  (float s) { return (vector3_t){ 0.0f,  0.0f,  s    }; }
static inline vector3_t vector3_back     (float s) { return (vector3_t){ 0.0f,  0.0f, -s    }; }

static inline vector4_t vector4_zero     (void)    { return (vector4_t){ 0.0f,  0.0f,  0.0f, 1.0f }; }
static inline vector4_t vector4_one      (float s) { return (vector4_t){ s,     s,     s,    1.0f }; }
static inline vector4_t vector4_up       (float s) { return (vector4_t){ 0.0f,  s,     0.0f, 1.0f }; }
static inline vector4_t vector4_down     (float s) { return (vector4_t){ 0.0f, -s,     0.0f, 1.0f }; }
static inline vector4_t vector4_right    (float s) { return (vector4_t){ s,     0.0f,  0.0f, 1.0f }; }
static inline vector4_t vector4_left     (float s) { return (vector4_t){-s,     0.0f,  0.0f, 1.0f }; }
static inline vector4_t vector4_forward  (float s) { return (vector4_t){ 0.0f,  0.0f,  s,    1.0f }; }
static inline vector4_t vector4_back     (float s) { return (vector4_t){ 0.0f,  0.0f, -s,    1.0f }; }

/*Prints a vec "v" using printf*/
static inline void 
vector2_print(const vector2_t v, const char *label) {
	printf("%s [%f, %f]\n", label, v.x, v.y);
}

/*Returns a vec that points in the opposite direction of the given vec
  "v". The vec returned has a magnitude identical to the given vec "v"*/
static inline vector2_t 
vector2_negate(vector2_t v) {
	return (vector2_t){.x = -v.x, .y = -v.y};
}

/*Subtracts a vec "subtrahend" from another vec "minuend"*/
static inline vector2_t 
vector2_subtract(vector2_t minuend, vector2_t subtrahend) {
	return (vector2_t){.x = minuend.x - subtrahend.x,
		.y = minuend.y - subtrahend.y};
}

/*Adds a vec "a" to another vec "b"*/
static inline vector2_t 
vector2_add(vector2_t a, vector2_t b) {
	return (vector2_t){.x = a.x + b.x, .y = a.y + b.y};
}

/*A more performant way of getting the relative length of a
  vec "v". This saves a square root operation making it more
  performant than vector2_Magnitude(). If all you have to do is
  compare a vecs length relatively, use this function instead of
  vector2_Magnitude()*/
static inline float 
vector2_square_magnitude(vector2_t v) {
	return ((v.x * v.x) + (v.y * v.y));
}

/*Returns the actual length of a vec "v".
  This uses a square root operation. Use vector2_SquareMagnitudenitude()
  to sacrifice accuracy and save on performance when comparing
  distances.*/
static inline float 
vector2_magnitude(vector2_t v) {
	return sqrt(vector2_square_magnitude(v));
}

/*Returns a given vec "v" as a unit vec.
  This means the magnitude(length) of the returned
  vec will always be 1 unit. The returned vec always points
  in the same direction as the given vec "v"*/
static inline vector2_t 
vector2_normalize(vector2_t v) {
	float m = vector2_magnitude(v);
	if (m == 0)
		return vector2_zero();
	return (vector2_t){.x = v.x / m, .y = v.y / m};
}

/*Returns the distance between point a and point b
  in units.*/
static inline float 
vector2_distance(vector2_t a, vector2_t b) {
	return vector2_magnitude(vector2_subtract(b, a));
}

/*Returns the distance squared between point a and point b
  in units. Faster than vector2_distance, but the result is the distance squared*/
static inline float 
vector2_square_distance(vector2_t a, vector2_t b) {
	return vector2_square_magnitude(vector2_subtract(b, a));
}

/*Scales a vec "v" by "scalar".
  increases the magnitude when "scalar" is greater than 1.
  decreases the magnitude when "scalar" is less than 0.
  The returned vec will point in the same direction as
  the given vec "v".*/
static inline vector2_t 
vector2_scale(vector2_t v, float scalar) {
	return (vector2_t){.x = v.x * scalar, .y = v.y * scalar};
}

/*For normalized vecs Dot returns 1 if they point in
  exactly the same direction, -1 if they point in completely opposite directions
  and zero if the vecs are perpendicular.*/
static inline float 
	vector2_dot(vector2_t a, vector2_t b) {
	return (a.x * b.x) + (a.y * b.y);
}

/* Returns the position of a physical body over time by using the given
 * 'acceleration' 'velocity' 'position' and 'time' arguments*/
static inline vector3_t 
vector3_kinematic_equation(
		vector3_t acceleration, 
		vector3_t velocity,
		vector3_t position, 
		float time) {
	float x = 0.5f * acceleration.x * time * time + velocity.x * time + position.x;
	float y = 0.5f * acceleration.y * time * time + velocity.y * time + position.y;
	float z = 0.5f * acceleration.z * time * time + velocity.z * time + position.z;
	return (vector3_t){x, y, z};
}

/*Linearly interpolates between "a" and "b" by "t".
If you want to make sure the returned value stays
between "a" and "b", use vector3_Lerpclamped() instead.
Returns a point at "t"% of the way between "a" and "b".*/
static inline vector2_t 
vector2_lerp(vector2_t a, vector2_t b, float t) {
	return (vector2_t){
		.x = a.x + (b.x - a.x) * t,
		.y = a.y + (b.y - a.y) * t,
	};
}

/*Linearly interpolates between "a" and "b" by "t".
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector2_t 
vector2_lerp_clamped(vector2_t a, vector2_t b, float t) {
	/*clamp n between 0 and 1*/
	t = t < 0.0f ? 0.0f : t;
	t = t > 1.0f ? 1.0f : t;
	/*perform lerp*/
	return (vector2_t){
		.x = a.x + (b.x - a.x) * t,
			.y = a.y + (b.y - a.y) * t,
	};
}

/*Converts a 2 dimensional vec to a 3 dimensional one*/
static inline vector3_t 
vector2_to_vector3(vector2_t v) {
	return (vector3_t){.x = v.x, .y = v.y, .z = 0.0f};
}

/*Converts a 2 dimensional vec to a 4 dimensional one*/
static inline vector4_t 
vector2_to_vector4(vector2_t v) {
	return (vector4_t){.x = v.x, .y = v.y, .z = 0.0f, .w = 1.0f};
}

/*Prints a vec "v" using printf*/
static inline void 
vector3_print(const vector3_t v, const char *label) {
	printf("%s [%f, %f, %f]\n", label, v.x, v.y, v.z);
}

/*Returns a vec that points in the opposite direction of the given vec
  "v". The vec returned has a magnitude identical to the given vec "v"*/
static inline vector3_t 
vector3_negate(vector3_t v) {
	return (vector3_t){.x = -v.x, .y = -v.y, .z = -v.z};
}

/*Adds a vec "a" to another vec "b"*/
static inline vector3_t 
vector3_add(vector3_t a, vector3_t b) {
	return (vector3_t){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

/*Subtracts a vec "subtrahend" from another vec "minuend"*/
static inline vector3_t 
vector3_subtract(vector3_t minuend, vector3_t subtrahend) {
	return (vector3_t){.x = minuend.x - subtrahend.x,
		.y = minuend.y - subtrahend.y,
		.z = minuend.z - subtrahend.z};
}

/*A more performant way of getting the relative length of a
  vec "v". This saves a square root operation making it more
  performant than vector3_Magnitude(). If all you have to do is
  compare a vecs length relatively, use this function instead of
  vector3_Magnitude()*/
static inline float 
vector3_square_magnitude(vector3_t v) {
	return ((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/*Returns the actual length of a vec "v".
  This uses a square root operation. Use vector3_SquareMagnitudenitude()
  to sacrifice accuracy and save on performance when comparing
  distances.*/
static inline float 
vector3_magnitude(vector3_t v) {
	return sqrt(vector3_square_magnitude(v));
}

/*Returns a given vec "v" as a unit vec.
  This means the magnitude(length) of the returned
  vec will always be 1 unit. The returned vec always points
  in the same direction as the given vec "v"*/
static inline vector3_t 
vector3_normalize(vector3_t v) {
	float m = vector3_magnitude(v);
	if (m == 0)
		return vector3_zero();
	return (vector3_t){.x = v.x / m, .y = v.y / m, .z = v.z / m};
}

/*Returns the distance between point a and point b
  in units.*/
static inline float 
vector3_distance(vector3_t a, vector3_t b) {
	return vector3_magnitude(vector3_subtract(b, a));
}

/*Returns the distance squared between point a and point b
  in units. Faster than vector3_distance, but the result is the distance squared*/
static inline float 
vector3_square_distance(vector3_t a, vector3_t b) {
	return vector3_square_magnitude(vector3_subtract(b, a));
}

/*Scales a vec "v" by "scalar".
  increases the magnitude when "scalar" is greater than 1.
  decreases the magnitude when "scalar" is less than 1.
  The returned vec will point in the same direction as
  the given vec "v".*/
static inline vector3_t 
vector3_scale(vector3_t v, float scalar) {
	return (vector3_t){.x = v.x * scalar, .y = v.y * scalar, .z = v.z * scalar};
}

/*Returns a vec parallel to both "a" and "b".*/
static inline vector3_t 
vector3_cross(vector3_t a, vector3_t b) {
	return (vector3_t){.x = (a.y * b.z) - (a.z * b.y),
		.y = -((a.x * b.z) - (a.z * b.x)),
		.z = (a.x * b.y) - (a.y * b.x)};
}

/*For normalized vecs Dot returns 1 if they point in
  exactly the same direction, -1 if they point in completely opposite directions
  and zero if the vecs are perpendicular.*/
static inline float 
vector3_dot(vector3_t a, vector3_t b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

/*Linearly interpolates between "a" and "b" by "t".
  If you want to make sure the returned value stays
  between "a" and "b", use vector2_Lerpclamped() instead.
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector3_t 
vector3_lerp(vector3_t a, vector3_t b, float t) {
	return (vector3_t){
		.x = a.x + (b.x - a.x) * t,
			.y = a.y + (b.y - a.y) * t,
			.z = a.z + (b.z - a.z) * t,
	};
}

/*Linearly interpolates between "a" and "b" by "t".
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector3_t 
vector3_lerp_clamped(vector3_t a, vector3_t b, float n) {
	/*clamp n between 0 and 1*/
	n = n < 0.0f ? 0.0f : n;
	n = n > 1.0f ? 1.0f : n;
	/*perform lerp*/
	return (vector3_t){
		.x = a.x + (b.x - a.x) * n,
			.y = a.y + (b.y - a.y) * n,
			.z = a.z + (b.z - a.z) * n,
	};
}

/*Returns a vec that is made from the largest components of two
  vecs.*/
static inline vector3_t 
vector3_max(vector3_t a, vector3_t b) {
	return (vector3_t){
		.x = a.x >= b.x ? a.x : b.x,
			.y = a.y >= b.y ? a.y : b.y,
			.z = a.z >= b.z ? a.z : b.z,
	};
}

/*Returns a vec that is made from the smallest components of two
  vecs.*/
static inline vector3_t 
vector3_min(vector3_t a, vector3_t b) {
	return (vector3_t){
		.x = a.x <= b.x ? a.x : b.x,
			.y = a.y <= b.y ? a.y : b.y,
			.z = a.z <= b.z ? a.z : b.z,
	};
}

/*Converts a 3 dimensional vec to a 2 dimensional one*/
static inline vector2_t 
vector3_to_vector2(vector3_t v) {
	return (vector2_t){.x = v.x, .y = v.y};
}

/*Converts a 3 dimensional vec to a 4 dimensional one*/
static inline vector4_t 
vector3_to_vector4(vector3_t v) {
	return (vector4_t){.x = v.x, .y = v.y, .z = v.z, .w = 1.0f};
}

/*Prints a vec "v" using printf*/
static inline void 
vector4_print(const vector4_t v, const char *label) {
	printf("%s [%f, %f, %f, %f]\n", label, v.x, v.y, v.z, v.w);
}

/*Returns a vec that points in the opposite direction of the given vec
  "v". The vec returned has a magnitude identical to the given vec "v"*/
static inline vector4_t 
vector4_negate(vector4_t v) {
	return (vector4_t){.x = -v.x, .y = -v.y, .z = -v.z, .w = -v.w};
}

/*A more performant way of getting the relative length of a
  vec "v". This saves a square root operation making it more
  performant than vector4_Magnitude(). If all you have to do is
  compare a vecs length relatively, use this function instead of
  vector4_Magnitude()*/
static inline float 
vector4_square_magnitude(vector4_t v) {
	return ((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}

/*Returns the actual length of a vec "v".
  This uses a square root operation. Use vector4_SquareMagnitudenitude()
  to sacrifice accuracy and save on performance when comparing
  distances.*/
static inline float 
vector4_magnitude(vector4_t v) {
	return sqrt(vector4_square_magnitude(v));
}

/*Returns a given vec "v" as a unit vec.
  This means the magnitude(length) of the returned
  vec will always be 1 unit. The returned vec always points
  in the same direction as the given vec "v"*/
static inline vector4_t 
vector4_normalize(vector4_t v) {
	float m = vector4_magnitude(v);
	if (m == 0)
		return vector4_zero();
	return (vector4_t){.x = v.x / m, .y = v.y / m, .z = v.z / m, .w = v.w / m};
}

/*Adds a vec "a" to another vec "b"*/
static inline vector4_t 
vector4_add(vector4_t a, vector4_t b) {
	return (vector4_t){
		.x = a.x + b.x,
			.y = a.y + b.y,
			.z = a.z + b.z,
			.w = a.w + b.w,
	};
}

/*Subtracts a vec "subtrahend" from another vec "minuend"*/
static inline vector4_t 
vector4_subtract(vector4_t minuend, vector4_t subtrahend) {
	return (vector4_t){
		.x = minuend.x - subtrahend.x,
			.y = minuend.y - subtrahend.y,
			.z = minuend.z - subtrahend.z,
			.w = minuend.w - subtrahend.w,
	};
}

/*Returns the distance between point a and point b
  in units.*/
static inline float 
vector4_distance(vector4_t a, vector4_t b) {
	return vector4_magnitude(vector4_subtract(b, a));
}

/*Returns the distance squared between point a and point b
  in units. Faster than vector4_distance, but the result is the distance squared*/
static inline float 
vector4_square_distance(vector4_t a, vector4_t b) {
	return vector4_square_magnitude(vector4_subtract(b, a));
}

/*Scales a vec "v" by "scalar".
  increases the magnitude when "scalar" is greater than 1.
  decreases the magnitude when "scalar" is less than 1.
  The returned vec will point in the same direction as
  the given vec "v".*/
static inline vector4_t 
vector4_scale(vector4_t v, float scalar) {
	return (vector4_t){
		.x = v.x * scalar,
			.y = v.y * scalar,
			.z = v.z * scalar,
			.w = v.w * scalar,
	};
}

/*For normalized vecs Dot returns 1 if they point in
  exactly the same direction, -1 if they point in completely opposite directions
  and zero if the vecs are perpendicular.*/
static inline float 
vector4_dot(vector4_t a, vector4_t b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

/*Linearly interpolates between "a" and "b" by "t".
  If you want to make sure the returned value stays
  between "a" and "b", use vector2_lerpclamped() instead.
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector4_t 
vector4_lerp(vector4_t a, vector4_t b, float t) {
	return (vector4_t){
		.x = a.x + (b.x - a.x) * t,
			.y = a.y + (b.y - a.y) * t,
			.z = a.z + (b.z - a.z) * t,
			.w = a.w + (b.w - a.w) * t,
	};
}

/*Linearly interpolates between "a" and "b" by "t".
  Returns a point at "t"% of the way between "a" and "b".*/
static inline vector4_t 
vector4_Lerpclamped(vector4_t a, vector4_t b, float n) {
	/*clamp n between 0 and 1*/
	n = n < 0.0f ? 0.0f : n;
	n = n > 1.0f ? 1.0f : n;
	/*perform lerp*/
	return (vector4_t){
		.x = a.x + (b.x - a.x) * n,
			.y = a.y + (b.y - a.y) * n,
			.z = a.z + (b.z - a.z) * n,
			.w = a.w + (b.w - a.w) * n,
	};
}

/*Returns a vec that is made from the largest components of two
  vecs.*/
static inline vector4_t 
vector4_Max(vector4_t a, vector4_t b) {
	return (vector4_t){
		.x = a.x >= b.x ? a.x : b.x,
			.y = a.y >= b.y ? a.y : b.y,
			.z = a.z >= b.z ? a.z : b.z,
			.w = a.w >= b.w ? a.w : b.w,
	};
}

/*Returns a vec that is made from the smallest components of two
  vecs.*/
static inline vector4_t 
vector4_Min(vector4_t a, vector4_t b) {
	return (vector4_t){
		.x = a.x <= b.x ? a.x : b.x,
			.y = a.y <= b.y ? a.y : b.y,
			.z = a.z <= b.z ? a.z : b.z,
			.w = a.w <= b.w ? a.w : b.w,
	};
}

/*Converts a 4 dimensional vec to a 2 dimensional one*/
static inline vector2_t 
vector4_Tovector2(vector4_t v) {
	return (vector2_t){.x = v.x, .y = v.y};
}

/*Converts a 4 dimensional vec to a 3 dimensional one*/
static inline vector3_t 
vector4_Tovector3(vector4_t v) {
	return (vector3_t){.x = v.x, .y = v.y, .z = v.z};
}

static inline matrix4_t 
matrix4_identity(void) {
	// clang-format off
	return (matrix4_t){
		.elements = {
			1.0f, 0.0f, 0.0f, 0.0f, 
			0.0f, 1.0f, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f, 0.0f, 
			0.0f, 0.0f, 0.0f, 1.0f
		}
	};
	// clang-format on
}

static inline matrix4_t 
matrix4_lookAt(vector3_t eye, vector3_t center, vector3_t up) {
	vector3_t f = vector3_normalize(vector3_subtract(eye, center));
	vector3_t s = vector3_normalize(vector3_cross(f, up));
	vector3_t u = vector3_cross(s, f);

	matrix4_t ret = matrix4_identity();
	ret.elements[0] = s.x;
	ret.elements[1] = u.x;
	ret.elements[2] = -f.x;
	ret.elements[3] = 0.0f;

	ret.elements[4] = s.y;
	ret.elements[5] = u.y;
	ret.elements[6] = -f.y;
	ret.elements[7] = 0.0f;

	ret.elements[8] = s.z;
	ret.elements[9] = u.z;
	ret.elements[10] = -f.z;
	ret.elements[11] = 0.0f;

	ret.elements[12] = -vector3_dot(s, eye);
	ret.elements[13] = -vector3_dot(u, eye);
	ret.elements[14] = vector3_dot(f, eye);
	ret.elements[15] = 1.0f;

	return ret;
}

/*Returns the difference between two 4x4 matrices.*/
static inline matrix4_t 
matrix4_Subtract(const matrix4_t min, const matrix4_t sub) {
	matrix4_t dif = matrix4_identity();
	int i = 0;

	for (i = 0; i < 16; i++)
		dif.elements[i] = min.elements[i] - sub.elements[i];

	return dif;
}

static inline matrix4_t 
matrix4_perspective(float fov, float aspect, float near, float far) {
	matrix4_t result = matrix4_identity();
	float Cotangent = (1.0f / tanf(fov / 2.0f));
	result.elements[0] = (Cotangent / aspect);
	result.elements[5] = Cotangent;
	result.elements[10] = -((near + far) / (near - far)); // negate for lh coords
	result.elements[11] = 1.0f;                           // negate for lh coords
	result.elements[14] = ((2.0f * near * far) / (near - far));
	return result;
}

/*Multiplies a 4x4 matrix with a 3 dimensional vec*/
static inline vector3_t 
matrix4_multiplyvector3(vector3_t Left, matrix4_t Right) {
	vector3_t result;
	result.y = Left.y * Right.elements[0];
	result.x = Left.y * Right.elements[4];
	result.z = Left.y * Right.elements[8];

	result.y += Left.x * Right.elements[1];
	result.x += Left.x * Right.elements[5];
	result.z += Left.x * Right.elements[9];

	result.y += Left.z * Right.elements[2];
	result.x += Left.z * Right.elements[6];
	result.z += Left.z * Right.elements[10];

	return result;
}

/*Multiplies a 4x4 matrix with a 4 dimensional vec*/
static inline vector4_t 
matrix4_multiplyvector4(vector4_t Left, matrix4_t Right) {
	vector4_t result;
	result.y = Left.y * Right.elements[0];
	result.x = Left.y * Right.elements[4];
	result.z = Left.y * Right.elements[8];
	result.w = Left.y * Right.elements[12];

	result.y += Left.x * Right.elements[1];
	result.x += Left.x * Right.elements[5];
	result.z += Left.x * Right.elements[9];
	result.w += Left.x * Right.elements[13];

	result.y += Left.z * Right.elements[2];
	result.x += Left.z * Right.elements[6];
	result.z += Left.z * Right.elements[10];
	result.w += Left.z * Right.elements[13];

	result.y += Left.w * Right.elements[3];
	result.x += Left.w * Right.elements[7];
	result.z += Left.w * Right.elements[11];
	result.w += Left.w * Right.elements[15];

	return result;
}

/*Multiplies a 4x4 matrix with another 4x4 matrix*/
static inline matrix4_t 
matrix4_multiply(const matrix4_t a, const matrix4_t b) {
	return (matrix4_t) {
		.elements = {
			// column 0
			a.elements[0] * b.elements[0] + a.elements[4] * b.elements[1] +
			a.elements[8] * b.elements[2] + a.elements[12] * b.elements[3],
			a.elements[0] * b.elements[1] + a.elements[1] * b.elements[5] +
			a.elements[2] * b.elements[9] + a.elements[3] * b.elements[13],
			a.elements[0] * b.elements[2] + a.elements[1] * b.elements[6] +
			a.elements[2] * b.elements[10] + a.elements[3] * b.elements[14],
			a.elements[0] * b.elements[3] + a.elements[1] * b.elements[7] +
			a.elements[2] * b.elements[11] + a.elements[3] * b.elements[15],

			// column 1
			a.elements[4] * b.elements[0] + a.elements[5] * b.elements[4] +
			a.elements[6] * b.elements[8] + a.elements[7] * b.elements[12],
			a.elements[4] * b.elements[1] + a.elements[5] * b.elements[5] +
			a.elements[6] * b.elements[9] + a.elements[7] * b.elements[13],
			a.elements[4] * b.elements[2] + a.elements[5] * b.elements[6] +
			a.elements[6] * b.elements[10] + a.elements[7] * b.elements[14],
			a.elements[4] * b.elements[3] + a.elements[5] * b.elements[7] +
			a.elements[6] * b.elements[11] + a.elements[7] * b.elements[15],

			// column 2
			a.elements[8] * b.elements[0] + a.elements[9] * b.elements[4] +
			a.elements[10] * b.elements[8] + a.elements[11] * b.elements[12],
			a.elements[8] * b.elements[1] + a.elements[9] * b.elements[5] +
			a.elements[10] * b.elements[9] + a.elements[11] * b.elements[13],
			a.elements[8] * b.elements[2] + a.elements[9] * b.elements[6] +
			a.elements[10] * b.elements[10] + a.elements[11] * b.elements[14],
			a.elements[8] * b.elements[3] + a.elements[9] * b.elements[7] +
			a.elements[10] * b.elements[11] + a.elements[11] * b.elements[15],

			// column 3
			a.elements[12] * b.elements[0] + a.elements[13] * b.elements[4] +
			a.elements[14] * b.elements[8] + a.elements[15] * b.elements[12],
			a.elements[12] * b.elements[1] + a.elements[13] * b.elements[5] +
			a.elements[14] * b.elements[9] + a.elements[15] * b.elements[13],
			a.elements[12] * b.elements[2] + a.elements[13] * b.elements[6] +
			a.elements[14] * b.elements[10] + a.elements[15] * b.elements[14],
			a.elements[12] * b.elements[3] + a.elements[13] * b.elements[7] +
			a.elements[14] * b.elements[11] +
			a.elements[15] * b.elements[15]
		}
	};
}

/*Scales (multiplies) a 4x4 matrix by a scalar (number)*/
static inline matrix4_t 
matrix4_scale(const vector3_t scale) {
	matrix4_t m = {0};
	m.elements[0]  = scale.x;
	m.elements[5]  = scale.y;
	m.elements[10] = scale.z;
	m.elements[15] = 1.0f;
	return m;
}

// returns a translation matrix from the specified vector4_t 'v'
static inline matrix4_t 
matrix4_translate(vector3_t v) {
	matrix4_t result = matrix4_identity();
	result.elements[12] = v.x;
	result.elements[13] = v.y;
	result.elements[14] = v.z;
	return result;
}

/*Prints a formatted matrix4_t to the console.*/
static inline void 
matrix4_print(matrix4_t m, const char *label) {
	// TODO condense this to one printf call.
	printf("--------------------------------\n");
	printf("matrix4: %s\n", label);
	printf("[0] %8f \t", m.elements[0]);
	printf("[4] %8f \t", m.elements[4]);
	printf("[8] %8f \t", m.elements[8]);
	printf("[12]%8f \t", m.elements[12]);
	printf("\n");
	printf("[1] %8f \t", m.elements[1]);
	printf("[5] %8f \t", m.elements[5]);
	printf("[9] %8f \t", m.elements[9]);
	printf("[13]%8f \t", m.elements[13]);
	printf("\n");
	printf("[2] %8f \t", m.elements[2]);
	printf("[6] %8f \t", m.elements[6]);
	printf("[10]%8f \t", m.elements[10]);
	printf("[14]%8f \t", m.elements[14]);
	printf("\n");
	printf("[3] %8f \t", m.elements[3]);
	printf("[7] %8f \t", m.elements[7]);
	printf("[11]%8f \t", m.elements[11]);
	printf("[15]%8f \t", m.elements[15]);
	printf("\n--------------------------------\n");
}

static inline quaternion_t 
quaternion_identity(void) {
	return (quaternion_t){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
}

static inline int 
quaternion_equal(quaternion_t a, quaternion_t b) {
	return
		fabs(a.x - b.x) <= FLOAT_EPSILON &&
		fabs(a.y - b.y) <= FLOAT_EPSILON &&
		fabs(a.z - b.z) <= FLOAT_EPSILON &&
		fabs(a.w - b.w) <= FLOAT_EPSILON;
}

static inline quaternion_t 
quaternion_from_angle_axis(float angle, vector3_t axis) {
	quaternion_t ret;
	float s = sinf(angle/2);
	ret.x = axis.x * s;
	ret.y = axis.y * s;
	ret.z = axis.z * s;
	ret.w = cosf(angle/2);
	return ret;
}

static inline float 
quaternion_magnitude(quaternion_t q) {
	return sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

static inline quaternion_t 
quaternion_normalize(quaternion_t q) {
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

static inline quaternion_t 
quaternion_conjugate(quaternion_t q) {
	return (quaternion_t) { 
		.w = q.w, 
			.x = -q.x, 
			.y = -q.y, 
			.z = -q.z 
	};
}

static inline quaternion_t 
quaternion_inverse(quaternion_t q) {
	return (quaternion_t) {
		.w = -q.w,
			.x = -q.x,
			.y = -q.y,
			.z = -q.z,
	};
}

static inline void 
quaternion_print(quaternion_t q, const char *label) {
	printf("\t%12f, %12f, %12f, %12f\t%s\n", q.x, q.y, q.z, q.w, label);
}

static inline quaternion_t 
quaternion_add(quaternion_t q1, quaternion_t q2) {
	return (quaternion_t) {
		.w = q1.w + q2.w,
			.x = q1.x + q2.x,
			.y = q1.y + q2.y,
			.z = q1.z + q2.z,
	};
}

static inline quaternion_t 
quaternion_multiply(quaternion_t q1, quaternion_t q2) {
	quaternion_t ret = {0};
	ret.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	ret.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	ret.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
	ret.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
	return ret;
}

static inline quaternion_t 
quaternion_scale(quaternion_t q, float scalar) {
	q.x *= scalar;
	q.y *= scalar;
	q.z *= scalar;
	q.w *= scalar;
	return q;
}

/*
   Returns the given vec 'v' rotated by the quaternion_t 'rotation'.
   */
static inline vector3_t
vector3_rotate(vector3_t v, quaternion_t rotation) {
	quaternion_t ret = (quaternion_t) {
		.w = 0,
			.x = v.x,
			.y = v.y,
			.z = v.z,
	};

	ret = quaternion_multiply(quaternion_multiply(rotation, ret), quaternion_conjugate(rotation));

	return (vector3_t) { 
		.x = ret.x, 
			.y = ret.y, 
			.z = ret.z,
	};
}

static inline quaternion_t 
quaternion_from_euler(vector3_t eulerAngles) {
	quaternion_t q;

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

static inline matrix4_t 
quaternion_to_matrix4(quaternion_t q) {
	matrix4_t mat = matrix4_identity();
	float* m = mat.elements;

	float xx = q.x * q.x;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float xw = q.x * q.w;

	float yy = q.y * q.y;
	float yz = q.y * q.z;
	float yw = q.y * q.w;

	float zz = q.z * q.z;
	float zw = q.z * q.w;

	m[0]  = 1 - 2 * ( yy + zz );
	m[4]  =     2 * ( xy - zw );
	m[8]  =     2 * ( xz + yw );

	m[1]  =     2 * ( xy + zw );
	m[5]  = 1 - 2 * ( xx + zz );
	m[9]  =     2 * ( yz - xw );

	m[2]  =     2 * ( xz - yw );
	m[6]  =     2 * ( yz + xw );
	m[10] = 1 - 2 * ( xx + yy );

	m[12] = m[13] = m[14] = m[3] = m[7] = m[11] = 0;
	m[15] = 1;

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

DEFINE_LIST(vector4_t)
DEFINE_LIST(vector3_t)
DEFINE_LIST(vector2_t)
DEFINE_LIST(quaternion_t)
DEFINE_LIST(matrix4_t)

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

#endif
