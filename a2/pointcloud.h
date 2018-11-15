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
  PointCloud(void) : num_points(0) {}

  // Load point cloud from a given .pts file
  void loadPointCloud(const char* filename);

  // Construct a KD-tree from all points
  void initKDTree(void);

  // Return the closest point in the point cloud to p
  Point nearest(Point &p) const;

  // Return a vector of num random points in the point cloud
  std::vector<Point>* randomPoints(int num) const;

  // Getters
  inline std::vector<Point>& getPoints(void) { return points; }
  inline const std::vector<Point>& getPoints(void) const { return points; }
  inline size_t size(void) { return num_points; }
  inline const size_t size(void) const { return num_points; }
  inline KDTree& getTree(void) { return tree; }
  inline const KDTree& getTree(void) const { return tree; }

private:
  // Return the closest point in the point cloud to p using brute-force search
  Point nearestBruteForce_(Point& p) const;

  // Return the closest point in the point cloud to p using a kd-tree
  Point nearestKDTree_(Point& p) const;

  std::vector<Point> points;
  KDTree tree;
  size_t num_points;
};

} // namespace icp

#endif
