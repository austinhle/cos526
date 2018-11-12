/* matrix6x6.cpp
 * Author: Austin Le
 * Simple library for a 6x6 matrix and a limited subset of operations.
*/

#include <cmath>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "matrix6x6.h"

using namespace std;

// Print out the matrix to stdout
void Matrix6x6::printMatrix(void) const {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      if (j < 5) {
        cout << (*this)(i, j) << " ";
      }
      else {
        cout << (*this)(i, j) << endl;
      }
    }
  }
}

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

       for(int k = 0; k < 6; k++)
       {
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
