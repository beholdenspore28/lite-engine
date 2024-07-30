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

#ifndef B_PHYSICS_H
#define B_PHYSICS_H

#include "blib/b_math.h"

typedef struct {
  bool is_intersecting;
  float distance;
} intersection_t;

typedef struct {
  vector3_t center;
  float radius;
} bounding_sphere_t;

//axis aligned bounding box
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
	
	return (intersection_t) {
		.is_intersecting = greatestDistance < 0, 
		.distance = greatestDistance,
	};
}

static inline intersection_t
bounding_sphere_intersect_sphere(bounding_sphere_t a, bounding_sphere_t b) {
	float radius_sum = a.radius + b.radius;
	float center_distance = vector3_distance(b.center, a.center);
	
	if (center_distance < radius_sum) {	
		return (intersection_t) {
			.is_intersecting = true,
			.distance = center_distance - radius_sum,
		};
	} else {
		return (intersection_t) {
			.is_intersecting = false,
			.distance = center_distance - radius_sum,
		};
	}
}

#endif /*B_PHYSICS_H*/
