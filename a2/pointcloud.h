/* pointcloud.h
 * Author: Austin Le
 * Simple library for 3D points and point clouds.
*/

#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include "vector3D.h"
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

  // Get Vector3D form of coordinates
  inline Vector3D toCoordsVector3D(void) const { return Vector3D(cx, cy, cz); }

  // Get Vector3D form of normal vector
  inline Vector3D toNormalVector3D(void) const { return Vector3D(nx, ny, nz); }

  // Get Vector4D form of coordinates
  inline Vector4D toCoordsVector4D(void) const { return Vector4D(cx, cy, cz); }

  // Get Vector4D form of normal vector
  inline Vector4D toNormalVector4D(void) const { return Vector4D(nx, ny, nz); }

  // Apply a rigid-body matrix transformation
  Point transform(const Matrix4x4& m) const;

  // Euclidian distance between this point and p
  inline double distanceTo(const Point& p) const {
    return sqrt(pow(cx - p.cx, 2) + pow(cy - p.cy, 2) + pow(cz - p.cz, 2));
  }

  // Output
  friend std::ostream& operator<<(std::ostream& os, const Point& p);
};

class PointCloud {
public:
  // Default constructor
  PointCloud(void) {}

  // Load point cloud from a given .pts file
  void loadPointCloud(const char* filename);

  // Return the closest point in the point cloud to p
  Point getClosestPoint(Point& p) const;

  // Return a vector of n random points in the point cloud
  std::vector<Point>* randomPoints(int n) const;

  // Getters
  inline std::vector<Point>& getPoints() { return points; }
  inline const std::vector<Point>& getPoints() const { return points; }
  inline size_t size() const { return n; }

private:
  std::vector<Point> points;
  size_t n;
};

#endif
