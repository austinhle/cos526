/* vector4D.cpp
 * Author: Austin Le
 * Simple library for a 4-dimensional vector and a limited subset of operations.
*/

#include <iostream>
#include "vector4D.h"

// Output
std::ostream& operator<<(std::ostream& os, const Vector4D& v) {
  os << "<" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ">";
  return os;
}
