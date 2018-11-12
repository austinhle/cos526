#include <cstdlib>
#include <vector>
#include <string>

#include "matrix4x4.h"
#include "vector4D.h"

class Point {
public:
  // Coordinates of the point
  double cx, cy, cz;

  // Vector components of the point's normal
  double nx, ny, nz;

  // Constructor
  Point(double cx_, double cy_, double cz_, double nx_, double ny_, double nz_) :
    cx(cx_), cy(cy_), cz(cz_), nx(nx_), ny(ny_), nz(nz_) {}

  // To string
  std::string toString(void) const;
};

class PointCloud {
public:
  // Default constructor
  PointCloud(void) {}

  // Utility functions
  void loadPointCloud(const char* filename);

  // Getters
  const std::vector<Point> &getPoints() const { return points; }
  size_t size() const { return n; }

private:
  std::vector<Point> points;
  size_t n;
};
