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
| Acceleration, angular  | α			| radian/T2	 | radian/s2   | radian/s2      |
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

#include "blib/bmath.h"
#include "blib/math3d.h"
