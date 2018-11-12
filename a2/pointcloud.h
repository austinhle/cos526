/* pointcloud.h
 * Author: Austin Le
 * Simple library for 3D points and point clouds.
*/

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

  // Default constructor
  Point(void) {}

  // Constructor
  Point(double cx_, double cy_, double cz_, double nx_, double ny_, double nz_) :
    cx(cx_), cy(cy_), cz(cz_), nx(nx_), ny(ny_), nz(nz_) {}

  // To string
  std::string toString(void) const;

  // Apply a rigid-body matrix transformation
  Point transform(const Matrix4x4& m) const;

  // Euclidian distance between this point and p
  double distanceTo(const Point& p) const;
};

class PointCloud {
public:
  // Default constructor
  PointCloud(void) {}

  // Load point cloud from a given .pts file
  void loadPointCloud(const char* filename);

  // Return the closest point in the point cloud to p
  Point getClosestPoint(Point& p) const;

  // Getters
  std::vector<Point>& getPoints() { return points; }
  const std::vector<Point>& getPoints() const { return points; }
  size_t size() const { return n; }

private:
  std::vector<Point> points;
  size_t n;
};
