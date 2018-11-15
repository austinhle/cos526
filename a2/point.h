/* point.h
 * Author: Austin Le
 * Interface for 3D point.
*/

#ifndef POINT_H
#define POINT_H

#include <iostream>

#include "vector3D.h"
#include "vector4D.h"
#include "matrix4x4.h"

namespace icp {

class Point {
public:
  // Default constructor
  Point(void) {}

  // Constructor
  Point(double cx_, double cy_, double cz_, double nx_, double ny_, double nz_) :
    cx(cx_), cy(cy_), cz(cz_), nx(nx_), ny(ny_), nz(nz_) {}

  // Getters
  inline double& operator[] (const size_t& index) {
    return (&cx)[index];
  }
  inline const double& operator[] (const size_t& index) const {
    return (&cx)[index];
  }

  // Get Vector3D form of coordinates
  inline Vector3D toCoordsVector3D(void) const { return Vector3D(cx, cy, cz); }

  // Get Vector3D form of normal vector
  inline Vector3D toNormalVector3D(void) const { return Vector3D(nx, ny, nz); }

  // Get Vector4D form of coordinates
  inline Vector4D toCoordsVector4D(void) const { return Vector4D(cx, cy, cz); }

  // Get Vector4D form of normal vector
  inline Vector4D toNormalVector4D(void) const { return Vector4D(nx, ny, nz); }

  // Apply a rigid-body matrix transformation
  Point transform(const Matrix4x4& m) const;

  // Euclidian distance between this point and p
  inline double distanceTo(const Point& p) const {
    return sqrt(pow(cx - p.cx, 2) + pow(cy - p.cy, 2) + pow(cz - p.cz, 2));
  }

  // Output
  friend std::ostream& operator<<(std::ostream& os, const Point& p);

private:
  // Coordinates of the point
  double cx, cy, cz;

  // Vector components of the point's normal
  double nx, ny, nz;
};

} // namespace icp

#endif
