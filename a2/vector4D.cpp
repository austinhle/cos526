/* vector4D.cpp
 * Author: Austin Le
 * Simple implementation of 4D vector.
*/

#include <iostream>
#include "vector4D.h"

namespace icp {

// Output
std::ostream& operator<<(std::ostream& os, const Vector4D& v) {
  os << "<" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ">";
  return os;
}

} // namespace icp
