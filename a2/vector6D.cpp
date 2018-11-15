/* vector6D.cpp
 * Author: Austin Le
 * Simple implementation of 6D vector.
*/

#include <iostream>
#include "vector6D.h"

namespace icp {

// Output
std::ostream& operator<<(std::ostream& os, const Vector6D& ov) {
  os << "<" << ov.x << ", " << ov.y << ", " << ov.z << ", "
            << ov.u << ", " << ov.v << ", " << ov.w << ">";
  return os;
}

} // namespace icp
