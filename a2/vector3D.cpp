/* vector3D.cpp
 * Author: Austin Le
 * Simple library for a 3-dimensional vector and a limited subset of operations.
*/

#include <iostream>
#include "vector3D.h"

// Output
std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
  os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
  return os;
}
