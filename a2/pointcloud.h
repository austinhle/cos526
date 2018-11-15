/* pointcloud.h
 * Author: Austin Le
 * Interface for 3D point cloud.
*/

#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <vector>
#include <string>

#include "point.h"
#include "kdtree.h"

namespace icp {

class PointCloud {
public:
  // Default constructor
  PointCloud(void) {}

  // Load point cloud from a given .pts file
  void loadPointCloud(const char* filename);

  // Construct a KD-tree from all points
  void initKDTree(void);

  // Return the closest point in the point cloud to p
  Point nearest(Point &p) const;

  // Return a vector of n random points in the point cloud
  std::vector<Point>* randomPoints(int n) const;

  // Getters
  inline std::vector<Point>& getPoints() { return points; }
  inline const std::vector<Point>& getPoints() const { return points; }
  inline size_t size() { return n; }
  inline const size_t size() const { return n; }
  inline KDTree& getTree() { return tree; }
  inline const KDTree& getTree() const { return tree; }

private:
  // Return the closest point in the point cloud to p using brute-force search
  Point nearestBruteForce_(Point& p) const;

  // Return the closest point in the point cloud to p using a kd-tree
  Point nearestKDTree_(Point& p) const;

  std::vector<Point> points;
  KDTree tree;
  size_t n;
};

} // namespace icp

#endif
