/* matrix4x4.h
 * Author: Austin Le
 * Simple library for a 4x4 matrix and operations.
*/

#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "vector4D.h"

class Matrix4x4 {
public:
  // Default constructor
  Matrix4x4(void) {}

  // Standard constructor
  // Assumes data is size 16
  Matrix4x4(double *data) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        (*this)(i, j) = data[i*4 + j];
      }
    }
  }

  static Matrix4x4 identity(void) {
    double d[] = {1., 0., 0., 0.,
                  0., 1., 0., 0.,
                  0., 0., 1., 0.,
                  0., 0., 0., 1.};
    return Matrix4x4(d);
  }

  // Load matrix from simple ASCII format file
  // Assumes that the file contains 16 values in the following format:
  // x x x x
  // x x x x
  // x x x x
  // x x x x
  void loadMatrix(const char* filename);

  // Print out the matrix to stdout
  void printMatrix(void) const;

  // Determinant
  double det(void) const;

  // Frobenius norm
  double norm(void) const;

  // Transpose
  Matrix4x4 transpose(void) const;

  // Inverse
  Matrix4x4 inverse(void) const;

  // Element access at index (i, j)
  double& operator()(int i, int j);
  const double& operator()(int i, int j) const;

  // Column access at column j
  Vector4D& operator[](int j);
  const Vector4D& operator[](int j) const;

  // Addition
  Matrix4x4 operator+(const Matrix4x4& m) const;

  // Addition and assignment
  void operator+=(const Matrix4x4& m);

  // Negation
  Matrix4x4 operator-(void) const;

  // Subtraction
  Matrix4x4 operator-(const Matrix4x4& m) const;

  // Subtraction and assignment
  void operator-=(const Matrix4x4& m);

  // Multiplication by scalar
  Matrix4x4 operator*(double c) const;

  // Multiplication by scalar and assignment
  void operator*=(double c);

  // Multiplication by vector
  Vector4D operator*(const Vector4D& v) const;

  // Matrix multiplication
  Matrix4x4 operator*(const Matrix4x4& m) const;

  // Division by scalar and assignment
  void operator/=(double c);

  friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& m);

private:
  Vector4D columns[4];
};

#endif
