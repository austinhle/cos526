/* matrix4x4.cpp
 * Author: Austin Le
 * Simple implementation of 4x4 matrix.
*/

#include <cmath>
#include <fstream>
#include <iostream>

#include "matrix4x4.h"

namespace icp {

// Load matrix from simple ASCII format file
// Assumes that the file contains 16 values in the following format:
// x x x x
// x x x x
// x x x x
// x x x x
void Matrix4x4::loadMatrix(const char* filename) {
  Matrix4x4& A(*this);
  double d1, d2, d3, d4;
  int count = 0;

  std::ifstream infile;
  std::string sfilename(filename);
  if (sfilename.compare(sfilename.size() - 3, 3, ".xf") != 0) {
    throw std::invalid_argument("File must be in .xf format");
  }

  infile.open(filename);

  if (infile.is_open()) {
    // Read 4 lines of 4 doubles each
    while ((count < 4) && (infile >> d1 >> d2 >> d3 >> d4)) {
      A(count, 0) = d1;
      A(count, 1) = d2;
      A(count, 2) = d3;
      A(count, 3) = d4;
      count++;
    }
    infile.close();
  } else {
    throw std::invalid_argument("File could not be opened");
  }
}

void Matrix4x4::saveMatrix(const char *filename) const {
  std::string fname(filename);
  std::string xfname = fname.substr(0, fname.size() - 4) + ".xf";

  std::ofstream outfile(xfname);
  if (outfile.is_open()) {
    // TODO: Write out the matrix!
    outfile << (*this);
    outfile.close();
  } else {
    throw std::invalid_argument("File could not be opened");
  }
}


// Determinant
double Matrix4x4::det(void) const {
  const Matrix4x4& A(*this);
	return
	  A(0,3)*A(1,2)*A(2,1)*A(3,0) - A(0,2)*A(1,3)*A(2,1)*A(3,0) -
	  A(0,3)*A(1,1)*A(2,2)*A(3,0) + A(0,1)*A(1,3)*A(2,2)*A(3,0) +
	  A(0,2)*A(1,1)*A(2,3)*A(3,0) - A(0,1)*A(1,2)*A(2,3)*A(3,0) -
	  A(0,3)*A(1,2)*A(2,0)*A(3,1) + A(0,2)*A(1,3)*A(2,0)*A(3,1) +
	  A(0,3)*A(1,0)*A(2,2)*A(3,1) - A(0,0)*A(1,3)*A(2,2)*A(3,1) -
	  A(0,2)*A(1,0)*A(2,3)*A(3,1) + A(0,0)*A(1,2)*A(2,3)*A(3,1) +
	  A(0,3)*A(1,1)*A(2,0)*A(3,2) - A(0,1)*A(1,3)*A(2,0)*A(3,2) -
	  A(0,3)*A(1,0)*A(2,1)*A(3,2) + A(0,0)*A(1,3)*A(2,1)*A(3,2) +
	  A(0,1)*A(1,0)*A(2,3)*A(3,2) - A(0,0)*A(1,1)*A(2,3)*A(3,2) -
	  A(0,2)*A(1,1)*A(2,0)*A(3,3) + A(0,1)*A(1,2)*A(2,0)*A(3,3) +
	  A(0,2)*A(1,0)*A(2,1)*A(3,3) - A(0,0)*A(1,2)*A(2,1)*A(3,3) -
	  A(0,1)*A(1,0)*A(2,2)*A(3,3) + A(0,0)*A(1,1)*A(2,2)*A(3,3);
}

// Frobenius norm
double Matrix4x4::norm(void) const {
  return sqrt(columns[0].norm2() +
              columns[1].norm2() +
              columns[2].norm2() +
              columns[3].norm2());
}

// Transpose
Matrix4x4 Matrix4x4::transpose(void) const {
  Matrix4x4 r;
  const Matrix4x4& A(*this);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r(i, j) = A(j, i);
    }
  }

  return r;
}

// Inverse
Matrix4x4 Matrix4x4::inverse(void) const {
  Matrix4x4 r;
  const Matrix4x4& A(*this);

	r(0,0) = A(1,2)*A(2,3)*A(3,1) - A(1,3)*A(2,2)*A(3,1) + A(1,3)*A(2,1)*A(3,2) - A(1,1)*A(2,3)*A(3,2) - A(1,2)*A(2,1)*A(3,3) + A(1,1)*A(2,2)*A(3,3);
	r(0,1) = A(0,3)*A(2,2)*A(3,1) - A(0,2)*A(2,3)*A(3,1) - A(0,3)*A(2,1)*A(3,2) + A(0,1)*A(2,3)*A(3,2) + A(0,2)*A(2,1)*A(3,3) - A(0,1)*A(2,2)*A(3,3);
	r(0,2) = A(0,2)*A(1,3)*A(3,1) - A(0,3)*A(1,2)*A(3,1) + A(0,3)*A(1,1)*A(3,2) - A(0,1)*A(1,3)*A(3,2) - A(0,2)*A(1,1)*A(3,3) + A(0,1)*A(1,2)*A(3,3);
	r(0,3) = A(0,3)*A(1,2)*A(2,1) - A(0,2)*A(1,3)*A(2,1) - A(0,3)*A(1,1)*A(2,2) + A(0,1)*A(1,3)*A(2,2) + A(0,2)*A(1,1)*A(2,3) - A(0,1)*A(1,2)*A(2,3);
	r(1,0) = A(1,3)*A(2,2)*A(3,0) - A(1,2)*A(2,3)*A(3,0) - A(1,3)*A(2,0)*A(3,2) + A(1,0)*A(2,3)*A(3,2) + A(1,2)*A(2,0)*A(3,3) - A(1,0)*A(2,2)*A(3,3);
	r(1,1) = A(0,2)*A(2,3)*A(3,0) - A(0,3)*A(2,2)*A(3,0) + A(0,3)*A(2,0)*A(3,2) - A(0,0)*A(2,3)*A(3,2) - A(0,2)*A(2,0)*A(3,3) + A(0,0)*A(2,2)*A(3,3);
	r(1,2) = A(0,3)*A(1,2)*A(3,0) - A(0,2)*A(1,3)*A(3,0) - A(0,3)*A(1,0)*A(3,2) + A(0,0)*A(1,3)*A(3,2) + A(0,2)*A(1,0)*A(3,3) - A(0,0)*A(1,2)*A(3,3);
	r(1,3) = A(0,2)*A(1,3)*A(2,0) - A(0,3)*A(1,2)*A(2,0) + A(0,3)*A(1,0)*A(2,2) - A(0,0)*A(1,3)*A(2,2) - A(0,2)*A(1,0)*A(2,3) + A(0,0)*A(1,2)*A(2,3);
	r(2,0) = A(1,1)*A(2,3)*A(3,0) - A(1,3)*A(2,1)*A(3,0) + A(1,3)*A(2,0)*A(3,1) - A(1,0)*A(2,3)*A(3,1) - A(1,1)*A(2,0)*A(3,3) + A(1,0)*A(2,1)*A(3,3);
	r(2,1) = A(0,3)*A(2,1)*A(3,0) - A(0,1)*A(2,3)*A(3,0) - A(0,3)*A(2,0)*A(3,1) + A(0,0)*A(2,3)*A(3,1) + A(0,1)*A(2,0)*A(3,3) - A(0,0)*A(2,1)*A(3,3);
	r(2,2) = A(0,1)*A(1,3)*A(3,0) - A(0,3)*A(1,1)*A(3,0) + A(0,3)*A(1,0)*A(3,1) - A(0,0)*A(1,3)*A(3,1) - A(0,1)*A(1,0)*A(3,3) + A(0,0)*A(1,1)*A(3,3);
	r(2,3) = A(0,3)*A(1,1)*A(2,0) - A(0,1)*A(1,3)*A(2,0) - A(0,3)*A(1,0)*A(2,1) + A(0,0)*A(1,3)*A(2,1) + A(0,1)*A(1,0)*A(2,3) - A(0,0)*A(1,1)*A(2,3);
	r(3,0) = A(1,2)*A(2,1)*A(3,0) - A(1,1)*A(2,2)*A(3,0) - A(1,2)*A(2,0)*A(3,1) + A(1,0)*A(2,2)*A(3,1) + A(1,1)*A(2,0)*A(3,2) - A(1,0)*A(2,1)*A(3,2);
	r(3,1) = A(0,1)*A(2,2)*A(3,0) - A(0,2)*A(2,1)*A(3,0) + A(0,2)*A(2,0)*A(3,1) - A(0,0)*A(2,2)*A(3,1) - A(0,1)*A(2,0)*A(3,2) + A(0,0)*A(2,1)*A(3,2);
	r(3,2) = A(0,2)*A(1,1)*A(3,0) - A(0,1)*A(1,2)*A(3,0) - A(0,2)*A(1,0)*A(3,1) + A(0,0)*A(1,2)*A(3,1) + A(0,1)*A(1,0)*A(3,2) - A(0,0)*A(1,1)*A(3,2);
	r(3,3) = A(0,1)*A(1,2)*A(2,0) - A(0,2)*A(1,1)*A(2,0) + A(0,2)*A(1,0)*A(2,1) - A(0,0)*A(1,2)*A(2,1) - A(0,1)*A(1,0)*A(2,2) + A(0,0)*A(1,1)*A(2,2);

  double d = det();
  if (d == 0.0) {
    throw std::overflow_error("Cannot invert; matrix is singular (has determinant 0).");
  }

  r /= d;

  return r;
}

// Element access at index (i, j)
double& Matrix4x4::operator()(int i, int j) {
  return columns[j][i];
}

const double& Matrix4x4::operator()(int i, int j) const {
  return columns[j][i];
}

// Column access at column j
Vector4D& Matrix4x4::operator[](int j) {
  return columns[j];
}

const Vector4D& Matrix4x4::operator[](int j) const {
  return columns[j];
}

// Addition
Matrix4x4 Matrix4x4::operator+(const Matrix4x4& m) const {
  Matrix4x4 r;
  const Matrix4x4& A(*this);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r(i, j) = A(i, j) + m(i, j);
    }
  }

  return r;
}

// Addition and assignment
void Matrix4x4::operator+=(const Matrix4x4& m) {
  Matrix4x4& A(*this);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      A(i, j) += m(i, j);
    }
  }
}

// Negation
Matrix4x4 Matrix4x4::operator-(void) const {
  Matrix4x4 r;
  const Matrix4x4& A(*this);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r(i, j) = -A(i, j);
    }
  }

  return r;
}

// Subtraction
Matrix4x4 Matrix4x4::operator-(const Matrix4x4& m) const {
  Matrix4x4 r;
  const Matrix4x4& A(*this);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r(i, j) = A(i, j) - m(i, j);
    }
  }

  return r;
}

// Subtraction and assignment
void Matrix4x4::operator-=(const Matrix4x4& m) {
  Matrix4x4& A(*this);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      A(i, j) -= m(i, j);
    }
  }
}

// Multiplication by scalar
Matrix4x4 Matrix4x4::operator*(double c) const {
  Matrix4x4 r;
  const Matrix4x4& A(*this);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r(i, j) = A(i, j) * c;
    }
  }

  return r;
}

// Multiplication by scalar and assignment
void Matrix4x4::operator*=(double c) {
  Matrix4x4& A(*this);
  double *Aij = (double *) &A;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      A(i, j) *= c;
    }
  }
}

// Multiplication by vector
Vector4D Matrix4x4::operator*(const Vector4D& v) const {
  return v[0]*columns[0] + v[1]*columns[1] + v[2]*columns[2] + v[3]*columns[3];
}

// Matrix multiplication
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m) const {
  Matrix4x4 r;
  const Matrix4x4& A(*this);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
       r(i, j) = 0.0;

       for(int k = 0; k < 4; k++) {
          r(i, j) += A(i, k) * m(k, j);
       }
    }
  }
  return r;
}

// Division by scalar and assignment
void Matrix4x4::operator/=(double c) {
  if (c == 0.0) {
    throw std::invalid_argument("Division by zero exception");
  }

  Matrix4x4& A(*this);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      A(i, j) /= c;
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Matrix4x4& m) {
  os << m(0, 0) << " " << m(0, 1) << " " << m(0, 2) << " " << m(0, 3) << std::endl;
  os << m(1, 0) << " " << m(1, 1) << " " << m(1, 2) << " " << m(1, 3) << std::endl;
  os << m(2, 0) << " " << m(2, 1) << " " << m(2, 2) << " " << m(2, 3) << std::endl;
  os << m(3, 0) << " " << m(3, 1) << " " << m(3, 2) << " " << m(3, 3) << std::endl;
  return os;
}

} // namespace icp
