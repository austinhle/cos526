/* matrix6x6.h
 * Author: Austin Le
 * Simple library for a 6x6 matrix and a limited subset of operations.
*/

#ifndef MATRIX6X6_H
#define MATRIX6X6_H

#include "vector6D.h"

class Matrix6x6 {
public:
  // Default constructor
  Matrix6x6(void) {}

  // Standard constructor
  // Assumes data is size 36
  Matrix6x6(double *data) {
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 6; j++) {
        (*this)(i, j) = data[i*6 + j];
      }
    }
  }

  static Matrix6x6 identity(void) {
    double d[] = {1., 0., 0., 0., 0., 0.,
                  0., 1., 0., 0., 0., 0.,
                  0., 0., 1., 0., 0., 0.,
                  0., 0., 0., 1., 0., 0.,
                  0., 0., 0., 0., 1., 0.,
                  0., 0., 0., 0., 0., 1.};
    return Matrix6x6(d);
  }

  // Print out the matrix to stdout
  void printMatrix(void) const;

  // Transpose
  Matrix6x6 transpose(void) const;

  // Element access at index (i, j)
  double& operator()(int i, int j);
  const double& operator()(int i, int j) const;

  // Column access at column j
  Vector6D& operator[](int j);
  const Vector6D& operator[](int j) const;

  // Addition
  Matrix6x6 operator+(const Matrix6x6& m) const;

  // Addition and assignment
  void operator+=(const Matrix6x6& m);

  // Negation
  Matrix6x6 operator-(void) const;

  // Subtraction
  Matrix6x6 operator-(const Matrix6x6& m) const;

  // Subtraction and assignment
  void operator-=(const Matrix6x6& m);

  // Multiplication by scalar
  Matrix6x6 operator*(double c) const;

  // Multiplication by scalar and assignment
  void operator*=(double c);

  // Multiplication by vector
  Vector6D operator*(const Vector6D& v) const;

  // Matrix multiplication
  Matrix6x6 operator*(const Matrix6x6& m) const;

  // Division by scalar and assignment
  void operator/=(double c);

  friend std::ostream& operator<<(std::ostream& os, const Matrix6x6& m);

private:
  Vector6D columns[6];
};

// Outer product of u and v
Matrix6x6 outer(const Vector6D& u, const Vector6D& v);

#endif
