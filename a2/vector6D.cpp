/* vector6D.cpp
 * Author: Austin Le
 * Simple library for a 6-dimensional vector and a limited subset of operations.
*/

#include <iostream>
#include "vector6D.h"

// Output
std::ostream& operator<<(std::ostream& os, const Vector6D& ov) {
  os << "<" << ov.x << ", " << ov.y << ", " << ov.z << ", "
            << ov.u << ", " << ov.v << ", " << ov.w << ">";
  return os;
}
