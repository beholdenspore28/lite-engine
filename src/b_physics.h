// clang-format off
/* Newton's Laws of Motion:
Law I
A body tends to remain at rest or continue to move in a straight line at constant
velocity unless acted upon by an external force. This is the so-called concept of
inertia.

Law II F = m * a = F = (M) (Length/Time2
Force is mass times acceleration which is itself equal to mass times length over time
 squared
The acceleration of a body is proportional to the resultant force acting on the body,
and this acceleration is in the same direction as the resultant force.

Law III
For every force acting on a body (action) there is an equal and opposite reacting
force (reaction), where the reaction is collinear to the acting force.

| Quantity               | Symbol       | Dimensions | Units, SI   | Units, English |
|====================================================================================
| Acceleration, linear   | A            | L/T2       | m/s2        | ft/s2          |
| Acceleration, angular  | α			      | radian/T2	 | radian/s2   | radian/s2      |
| Density                | ρ            | M/L3       | kg/m3       | slug/ft3       |
| Force                  | F            | M (L/T2)   | newton,N    | pound, lbs     |
| Kinematic viscosity    | ν            | L2/T       | m2/s        | ft2/s          |
| Length                 | L (or x,y,z) | L          | meters, m   | feet, ft       |
| Mass                   | m            | M          | kilogram, k | gslug          |
| Moment (torque)        | Ma           | M (L2/T2)  | N-m         | ft-lbs         |
| Mass Moment of Inertia | I            | ML2        | kg-m2       | lbs-ft-s2      |
| Pressure               | P            | M/(L T2)   | N/m2        | lbs/ft2        |
| Time                   | T            | T          | seconds, s  | seconds, s     |
| Velocity, linear       | V            | L/T        | m/s         | ft/s           |
| Velocity, angular      | w            | radian/T   | radian/s    | radian/s       |
| Viscosity              | μ            | M/(L T)    | N s/m2      | lbs • s/ft2    |
=====================================================================================

With regard to notation, we’ll use boldface type to indicate a vector quantity, such as
force, F. When referring to the magnitude only of a vector quantity, we’ll use standard
type. For example, the magnitude of the vector force, F, is F with components along the
coordinate axes, Fx, Fy, and Fz. In the code samples throughout the book, we’ll use the
* (asterisk) to indicate vector dot product, or scalar product, operations depending on
the context, and we’ll use the ^ (caret) to indicate vector cross product.
*/
// clang-format on

#pragma once

#include "blib/b_math.h"

typedef struct {
  bool is_intersecting;
  float distance;
} intersection_t;

typedef struct {
  vector3_t center;
  float radius;
} bounding_sphere_t;

// axis aligned bounding box
typedef struct {
  vector3_t min_extents;
  vector3_t max_extents;
} aligned_box_t;

static inline intersection_t
aligned_box_intersect_aligned_box(aligned_box_t a, aligned_box_t b) {
  vector3_t diff1 = vector3_subtract(b.min_extents, a.max_extents);
  vector3_t diff2 = vector3_subtract(a.min_extents, b.max_extents);
  vector3_t diff = vector3_max(diff1, diff2);

  float greatestDistance = diff.x >= diff.y ? diff.x : diff.y;
  greatestDistance = diff.z >= diff.x ? diff.z : diff.x;

  return (intersection_t){
      .is_intersecting = greatestDistance < 0,
      .distance = greatestDistance,
  };
}

static inline intersection_t
bounding_sphere_intersect_sphere(bounding_sphere_t a, bounding_sphere_t b) {
  float radius_sum = a.radius + b.radius;
  float center_distance = vector3_distance(b.center, a.center);

  if (center_distance < radius_sum) {
    return (intersection_t){
        .is_intersecting = true,
        .distance = center_distance - radius_sum,
    };
  } else {
    return (intersection_t){
        .is_intersecting = false,
        .distance = center_distance - radius_sum,
    };
  }
}

#if 0
static void aligned_box_test(void) {
	aligned_box_t box1 = { .min_extents = { 0.0f, 0.0f, 0.0f }, .max_extents = { 1.0f, 1.0f, 1.0f },};
	aligned_box_t box2 = { .min_extents = { 1.0f, 1.0f, 1.0f }, .max_extents = { 2.0f, 2.0f, 2.0f },};
	aligned_box_t box3 = { .min_extents = { 1.0f, 0.0f, 0.0f }, .max_extents = { 2.0f, 1.0f, 1.0f },};
	aligned_box_t box4 = { .min_extents = { 0.0f, 0.0f,-2.0f }, .max_extents = { 1.0f, 1.0f,-1.0f },};
	aligned_box_t box5 = { .min_extents = { 0.0f, 0.5f, 0.0f }, .max_extents = { 1.0f, 1.5f, 1.0f },};

	intersection_t b1b2 = aligned_box_intersect_aligned_box(box1, box2);
	intersection_t b1b3 = aligned_box_intersect_aligned_box(box1, box3);
	intersection_t b1b4 = aligned_box_intersect_aligned_box(box1, box4);
	intersection_t b1b5 = aligned_box_intersect_aligned_box(box1, box5);

	printf("b1b2: %d %f\n", b1b2.is_intersecting, b1b2.distance);
	printf("b1b3: %d %f\n", b1b3.is_intersecting, b1b3.distance);
	printf("b1b4: %d %f\n", b1b4.is_intersecting, b1b4.distance);
	printf("b1b5: %d %f\n", b1b5.is_intersecting, b1b5.distance);

	return 0;
}

static void bounding_sphere_test(void) {
	bounding_sphere_t sphere1 = {
		.center = vector3_zero(),
		.radius = 1.0f,
	};
	bounding_sphere_t sphere2 = {
		.center = vector3_up(3.0),
		.radius = 1.0f,
	};
	bounding_sphere_t sphere3 = {
		.center = vector3_forward(2.0),
		.radius = 1.0f,
	};
	bounding_sphere_t sphere4 = {
		.center = vector3_right(1.0),
		.radius = 1.0f,
	};

	intersection_t i2 = bounding_sphere_intersect_sphere(sphere1, sphere2);
	intersection_t i3 = bounding_sphere_intersect_sphere(sphere1, sphere3);
	intersection_t i4 = bounding_sphere_intersect_sphere(sphere1, sphere4);

	printf("i2: %d %f\n", i2.is_intersecting, i2.distance);
	printf("i3: %d %f\n", i3.is_intersecting, i3.distance);
	printf("i4: %d %f\n", i4.is_intersecting, i4.distance);
	return 0;
}
#endif
