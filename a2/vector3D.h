/* vector3D.h
 * Author: Austin Le
 * Interface for 3D vector.
*/

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <iostream>

namespace icp {

class Vector3D {
public:
  double x, y, z;

  // Constructors
  Vector3D(void) : x(0.0), y(0.0), z(0.0) {}
  Vector3D(double x_, double y_, double z_) :
    x(x_), y(y_), z(z_) {}
  Vector3D(double c) :
    x(c), y(c), z(c) {}

  // Accessors
  inline double& operator[] (const int& index) {
    return (&x)[index];
  }
  inline const double& operator[] (const int& index) const {
    return (&x)[index];
  }

  // Negation
  inline Vector3D operator-(void) const {
    return Vector3D(-x, -y, -z);
  }

  // Addition
  inline Vector3D operator+(const Vector3D& v) const {
    return Vector3D(x + v.x, y + v.y, z + v.z);
  }

  // Subtraction
  inline Vector3D operator-(const Vector3D& v) const {
    return Vector3D(x - v.x, y - v.y, z - v.z);
  }

  // Right scalar multiplication
  inline Vector3D operator*(const double& c) const {
    return Vector3D(x * c, y * c, z * c);
  }

  // Scalar division
  inline Vector3D operator/(const double& c) const {
    const double rc = 1.0/c;
    return Vector3D(rc * x, rc * y, rc * z);
  }

  // Addition and assignment
  inline void operator+=(const Vector3D& v) {
    x += v.x; y += v.y; z += v.z;
  }

  // Subtraction and assignment
  inline void operator-=(const Vector3D& v) {
    x -= v.x; y -= v.y; z -= v.z;
  }

  // Scalar multiplication and assignment
  inline void operator*=(const double& c) {
    x *= c; y *= c; z *= c;
  }

  // Scalar division / assignment
  inline void operator/=(const double& c) {
    (*this) *= (1.0/c);
  }

  // 6-dimensional Euclidian distance
  inline double norm(void) const {
    return sqrt(x*x + y*y + z*z);
  }

  // 6-dimensional Euclidian length squared
  inline double norm2(void) const {
    return x*x + y*y + z*z;
  }

  friend std::ostream& operator<<(std::ostream& os, const Vector3D& v);
};

// Left scalar multiplication
inline Vector3D operator*(const double& c, const Vector3D& v) {
  return Vector3D(c * v.x, c * v.y, c * v.z);
}

// Inner (dot) product
inline double dot(const Vector3D& u, const Vector3D& v) {
  return u.x*v.x + u.y*v.y + u.z*v.z;
}

// Outer (cross) product
inline Vector3D cross(const Vector3D& u, const Vector3D& v) {
  return Vector3D(u.y*v.z - u.z*v.y,
                  u.z*v.x - u.x*v.z,
                  u.x*v.y - u.y*v.x);
}

} // namespace icp

#endif
