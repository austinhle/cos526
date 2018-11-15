/* vector3D.cpp
 * Author: Austin Le
 * Simple implementation of 3D vector.
*/

#include <iostream>
#include "vector3D.h"

namespace icp {

// Output
std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
  os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
  return os;
}

} // namespace icp
