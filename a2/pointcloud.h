/* pointcloud.h
 * Author: Austin Le
 * Simple library for 3D point clouds.
*/

#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include "point.h"
#include "kdtree.h"
#include "vector3D.h"
#include "matrix4x4.h"
#include "vector4D.h"

class PointCloud {
public:
  // Default constructor
  PointCloud(void) {}

  // Load point cloud from a given .pts file
  void loadPointCloud(const char* filename);

  // Construct a KD-tree from all points
  void initKDTree(void);

  // Return the closest point in the point cloud to p using brute-force search
  Point nearestBruteForce(Point& p) const;

  // Return the closest point in the point cloud to p using a kd-tree
  Point nearestKDTree(Point& p) const;

  // Return a vector of n random points in the point cloud
  std::vector<Point>* randomPoints(int n) const;

  // Getters
  inline std::vector<Point>& getPoints() { return points; }
  inline const std::vector<Point>& getPoints() const { return points; }
  inline size_t size() const { return n; }
  inline KDTree& getTree() { return tree; }
  inline const KDTree& getTree() const { return tree; }

private:
  std::vector<Point> points;
  KDTree tree;
  size_t n;
};

#endif
