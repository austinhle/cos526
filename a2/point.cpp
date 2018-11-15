/* point.cpp
 * Author: Austin Le
 * Simple implementation of 3D point.
*/

#include <iostream>

#include "point.h"
#include "matrix4x4.h"
#include "vector4D.h"

namespace icp {

Point Point::transform(const Matrix4x4& m) const {
  Vector4D v = toCoordsVector4D();
  Vector4D t = m * v;
  Vector4D n = toNormalVector4D();
  Vector4D nt = m * n;

  return Point(t.x, t.y, t.z, nt.x, nt.y, nt.z);
}

std::ostream& operator<<(std::ostream& os, const Point& p) {
  os << "<" << p.cx << ", " << p.cy << ", " << p.cz << ", "
            << p.nx << ", " << p.ny << ", " << p.nz << ">";
  return os;
}

} // namespace icp
