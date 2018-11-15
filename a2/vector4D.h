/* vector4D.h
 * Author: Austin Le
 * Interface for 4D vector.
*/

#ifndef VECTOR4D_H
#define VECTOR4D_H

#include <cmath>
#include <iostream>

namespace icp {

class Vector4D {
public:
  double x, y, z, w;

  // Constructors
  Vector4D(void) : x(0.0), y(0.0), z(0.0), w(0.0) {}
  Vector4D(double x_, double y_, double z_, double w_) :
    x(x_), y(y_), z(z_), w(w_) {}
  Vector4D(double x_, double y_, double z_) :
    x(x_), y(y_), z(z_), w(1.0) {}
  Vector4D(double c) :
    x(c), y(c), z(c), w(c) {}

  // Accessors
  inline double& operator[] (const int& index) {
    return (&x)[index];
  }
  inline const double& operator[] (const int& index) const {
    return (&x)[index];
  }

  // Negation
  inline Vector4D operator-(void) const {
    return Vector4D(-x, -y, -z, -w);
  }

  // Addition
  inline Vector4D operator+(const Vector4D& v) const {
    return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
  }

  // Subtraction
  inline Vector4D operator-(const Vector4D& v) const {
    return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
  }

  // Right scalar multiplication
  inline Vector4D operator*(const double& c) const {
    return Vector4D(x * c, y * c, z * c, w * c);
  }

  // Scalar division
  inline Vector4D operator/(const double& c) const {
    const double rc = 1.0/c;
    return Vector4D(rc * x, rc * y, rc * z, rc * w);
  }

  // Addition and assignment
  inline void operator+=(const Vector4D& v) {
    x += v.x; y += v.y; z += v.z; z += v.w;
  }

  // Subtraction and assignment
  inline void operator-=(const Vector4D& v) {
    x -= v.x; y -= v.y; z -= v.z; w -= v.w;
  }

  // Scalar multiplication and assignment
  inline void operator*=(const double& c) {
    x *= c; y *= c; z *= c; w *= c;
  }

  // Scalar division / assignment
  inline void operator/=(const double& c) {
    (*this) *= (1.0/c);
  }

  // 4-dimensional Euclidian distance
  inline double norm(void) const {
    return sqrt(x*x + y*y + z*z + w*w);
  }

  // 4-dimensional Euclidian length squared
  inline double norm2(void) const {
    return x*x + y*y + z*z + w*w;
  }

  friend std::ostream& operator<<(std::ostream& os, const Vector4D& v);
};

// Left scalar multiplication
inline Vector4D operator*(const double& c, const Vector4D& v) {
  return Vector4D(c*v.x, c*v.y, c*v.z, c*v.w);
}

// Inner (dot) product
inline double dot(const Vector4D& u, const Vector4D& v) {
  return u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w;
}

} // namespace icp

#endif
