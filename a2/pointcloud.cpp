/* pointcloud.cpp
 * Author: Austin Le
 * Simple implementation of a 3D point cloud.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>

#include "kdtree.h"
#include "point.h"
#include "pointcloud.h"
#include "vector3D.h"
#include "matrix4x4.h"
#include "vector4D.h"

namespace icp {

void PointCloud::loadPointCloud(const char *filename) {
  double cx, cy, cz, nx, ny, nz;
  std::ifstream infile;
  std::string sfilename(filename);
  if (sfilename.compare(sfilename.size() - 4, 4, ".pts") != 0) {
    throw std::invalid_argument("File must be in .pts format");
  }
  infile.open(filename);

  if (infile.is_open()) {
    // Read a line of 6 doubles at a time
    while (infile >> cx >> cy >> cz >> nx >> ny >> nz) {
      points.push_back(Point(cx, cy, cz, nx, ny, nz));
      num_points++;
    }
    infile.close();
  } else {
    throw std::invalid_argument("File could not be opened");
  }
}

void PointCloud::initKDTree(void) {
  for (size_t i = 0; i < points.size(); i++) {
    tree.insert(points[i]);
  }
}

Point PointCloud::nearest(Point &p) const {
  return nearestKDTree_(p);
}

Point PointCloud::nearestBruteForce_(Point& p) const {
  double testDist;
  double closestDist = INFINITY;
  Point closestPoint;
  const std::vector<Point>& points = getPoints();

  for (const Point& p2 : points) {
    testDist = p.distanceTo(p2);
    if (testDist < closestDist) {
      closestPoint = p2;
      closestDist = testDist;
    }
  }

  return closestPoint;
}

Point PointCloud::nearestKDTree_(Point& p) const {
  return tree.nearestPoint(p);
}

// Adapted from https://www.gnu.org/software/gsl/doc/html/permutation.html
std::vector<Point> *PointCloud::randomPoints(int num) const {
  gsl_rng *r;
  gsl_permutation *p;
  std::vector<Point> *randomPts = new std::vector<Point>(num);
  const std::vector<Point> allPts = getPoints();

  // Initialize random number generator
  gsl_rng_env_setup();
  r = gsl_rng_alloc(gsl_rng_default);

  // Use current time in nanoseconds as the seed
  struct timespec tm;
  clock_gettime(CLOCK_REALTIME, &tm);
  gsl_rng_set(r, tm.tv_nsec);

  // Initialize permutation
  p = gsl_permutation_alloc(num_points);
  gsl_permutation_init(p);

  // Create random permutation
  gsl_ran_shuffle(r, p->data, num_points, sizeof(size_t));

  // Use values from random permutation to select numPts
  for (size_t i = 0; i < num; i++) {
    (*randomPts)[i] = (allPts[p->data[i]]);
  }

  // Cleanup
  gsl_permutation_free(p);
  gsl_rng_free(r);

  return randomPts;
}

} // namespace icp
