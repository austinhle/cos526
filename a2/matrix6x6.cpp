/* matrix6x6.cpp
 * Author: Austin Le
 * Simple implementation of 6x6 matrix.
*/

#include <cmath>
#include <fstream>
#include <iostream>

#include "matrix6x6.h"
#include "vector6D.h"

namespace icp {

// Transpose
Matrix6x6 Matrix6x6::transpose(void) const {
  Matrix6x6 r;
  const Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      r(i, j) = A(j, i);
    }
  }

  return r;
}

// Element access at index (i, j)
double& Matrix6x6::operator()(int i, int j) {
  return columns[j][i];
}

const double& Matrix6x6::operator()(int i, int j) const {
  return columns[j][i];
}

// Column access at column j
Vector6D& Matrix6x6::operator[](int j) {
  return columns[j];
}

const Vector6D& Matrix6x6::operator[](int j) const {
  return columns[j];
}

// Addition
Matrix6x6 Matrix6x6::operator+(const Matrix6x6& m) const {
  Matrix6x6 r;
  const Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      r(i, j) = A(i, j) + m(i, j);
    }
  }

  return r;
}

// Addition and assignment
void Matrix6x6::operator+=(const Matrix6x6& m) {
  Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      A(i, j) += m(i, j);
    }
  }
}

// Negation
Matrix6x6 Matrix6x6::operator-(void) const {
  Matrix6x6 r;
  const Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      r(i, j) = -A(i, j);
    }
  }

  return r;
}

// Subtraction
Matrix6x6 Matrix6x6::operator-(const Matrix6x6& m) const {
  Matrix6x6 r;
  const Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      r(i, j) = A(i, j) - m(i, j);
    }
  }

  return r;
}

// Subtraction and assignment
void Matrix6x6::operator-=(const Matrix6x6& m) {
  Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      A(i, j) -= m(i, j);
    }
  }
}

// Multiplication by scalar
Matrix6x6 Matrix6x6::operator*(double c) const {
  Matrix6x6 r;
  const Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      r(i, j) = A(i, j) * c;
    }
  }

  return r;
}

// Multiplication by scalar and assignment
void Matrix6x6::operator*=(double c) {
  Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      A(i, j) *= c;
    }
  }
}

// Multiplication by vector
Vector6D Matrix6x6::operator*(const Vector6D& v) const {
  return v[0]*columns[0] + v[1]*columns[1] + v[2]*columns[2] +
         v[3]*columns[3] + v[4]*columns[4] + v[5]*columns[5];
}

// Matrix multiplication
Matrix6x6 Matrix6x6::operator*(const Matrix6x6& m) const {
  Matrix6x6 r;
  const Matrix6x6& A(*this);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
       r(i, j) = 0.0;

       for(int k = 0; k < 6; k++) {
          r(i, j) += A(i, k) * m(k, j);
       }
    }
  }
  return r;
}

// Division by scalar and assignment
void Matrix6x6::operator/=(double c) {
  if (c == 0.0) {
    throw std::invalid_argument("Division by zero exception");
  }

  Matrix6x6& A(*this);
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      A(i, j) /= c;
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Matrix6x6& m) {
  os << m(0, 0) << " " << m(0, 1) << " " << m(0, 2) << " "
     << m(0, 3) << " " << m(0, 4) << " " << m(0, 5) << std::endl;
  os << m(1, 0) << " " << m(1, 1) << " " << m(1, 2) << " "
     << m(1, 3) << " " << m(1, 4) << " " << m(1, 5) << std::endl;
  os << m(2, 0) << " " << m(2, 1) << " " << m(2, 2) << " "
     << m(2, 3) << " " << m(2, 4) << " " << m(2, 5) << std::endl;
  os << m(3, 0) << " " << m(3, 1) << " " << m(3, 2) << " "
     << m(3, 3) << " " << m(3, 4) << " " << m(3, 5) << std::endl;
  os << m(4, 0) << " " << m(4, 1) << " " << m(4, 2) << " "
     << m(4, 3) << " " << m(4, 4) << " " << m(4, 5) << std::endl;
  os << m(5, 0) << " " << m(5, 1) << " " << m(5, 2) << " "
     << m(5, 3) << " " << m(5, 4) << " " << m(5, 5) << std::endl;
  return os;
}

// Outer product of u and v.
Matrix6x6 outer(const Vector6D& u, const Vector6D& v) {
  Matrix6x6 r;

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      r(i, j) = u[i] * v[j];
    }
  }

  return r;
}

} // namespace icp
