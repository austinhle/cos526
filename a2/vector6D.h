/* vector6D.h
 * Author: Austin Le
 * Simple library for a 6-dimensional vector and a limited subset of operations.
*/

#ifndef VECTOR6D_H
#define VECTOR6D_H

#include <cmath>

class Vector6D {
public:
  double x, y, z, u, v, w;

  // Constructors
  Vector6D(void) : x(0.0), y(0.0), z(0.0), u(0.0), v(0.0), w(0.0) {}
  Vector6D(double x_, double y_, double z_, double u_, double v_, double w_) :
    x(x_), y(y_), z(z_), u(u_), v(v_), w(w_) {}
  Vector6D(double c) :
    x(c), y(c), z(c), u(c), v(c), w(c) {}

  // Accessors
  inline double& operator[] (const int& index) {
    return (&x)[index];
  }
  inline const double& operator[] (const int& index) const {
    return (&x)[index];
  }

  // Negation
  inline Vector6D operator-(void) const {
    return Vector6D(-x, -y, -z, -u, -v, -w);
  }

  // Addition
  inline Vector6D operator+(const Vector6D& ov) const {
    return Vector6D(x + ov.x, y + ov.y, z + ov.z, u + ov.u, v + ov.v, w + ov.w);
  }

  // Subtraction
  inline Vector6D operator-(const Vector6D& ov) const {
    return Vector6D(x - ov.x, y - ov.y, z - ov.z, u - ov.u, v - ov.v, w - ov.w);
  }

  // Right scalar multiplication
  inline Vector6D operator*(const double& c) const {
    return Vector6D(x * c, y * c, z * c, u * c, v * c, w * c);
  }

  // Scalar division
  inline Vector6D operator/(const double& c) const {
    const double rc = 1.0/c;
    return Vector6D(rc * x, rc * y, rc * z, rc * u, rc * v, rc * w);
  }

  // Addition and assignment
  inline void operator+=(const Vector6D& ov) {
    x += ov.x; y += ov.y; z += ov.z; u += ov.u; v += ov.v; w += ov.w;
  }

  // Subtraction and assignment
  inline void operator-=(const Vector6D& ov) {
    x -= ov.x; y -= ov.y; z -= ov.z; u -= ov.u; v -= ov.v; w -= ov.w;
  }

  // Scalar multiplication and assignment
  inline void operator*=(const double& c) {
    x *= c; y *= c; z *= c; u *= c; v *= c; w *= c;
  }

  // Scalar division / assignment
  inline void operator/=(const double& c) {
    (*this) *= (1.0/c);
  }

  // 6-dimensional Euclidian distance
  inline double norm(void) const {
    return sqrt(x*x + y*y + z*z + u*u + v*v + w*w);
  }

  // 6-dimensional Euclidian length squared
  inline double norm2(void) const {
    return x*x + y*y + z*z + u*u + v*v + w*w;
  }
};

// Left scalar multiplication
inline Vector6D operator*(const double& c, const Vector6D& v) {
  return Vector6D(c*v.x, c*v.y, c*v.z, c*v.u, c*v.v, c*v.w);
}

// Inner (dot) product
inline double dot(const Vector6D& u, const Vector6D& v) {
  return u.x*v.x + u.y*v.y + u.z*v.z + u.u*v.u + u.v*v.v + u.w*v.w;
}

#endif
