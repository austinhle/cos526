/* pointcloud.cpp
 * Author: Austin Le
 * Simple library for 3D points and point clouds.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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

using namespace std;

void PointCloud::loadPointCloud(const char *filename) {
  double cx, cy, cz, nx, ny, nz;
  ifstream infile;
  string sfilename(filename);
  if (sfilename.compare(sfilename.size() - 4, 4, ".pts") != 0) {
    throw std::invalid_argument("File must be in .pts format");
  }
  infile.open(filename);

  // Initialize instance variable to count number of points
  n = 0;

  if (infile.is_open()) {
    // Read a line of 6 doubles at a time
    while (infile >> cx >> cy >> cz >> nx >> ny >> nz) {
      points.push_back(Point(cx, cy, cz, nx, ny, nz));
      n++;
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

Point PointCloud::nearestBruteForce(Point& p) const {
  double testDist;
  double closestDist = INFINITY;
  Point closestPoint;
  const vector<Point>& points = getPoints();

  for (const Point& p2 : points) {
    testDist = p.distanceTo(p2);
    if (testDist < closestDist) {
      closestPoint = p2;
      closestDist = testDist;
    }
  }

  return closestPoint;
}

Point PointCloud::nearestKDTree(Point& p) const {
  return tree.nearestPoint(p);
}

// Adapted from https://www.gnu.org/software/gsl/doc/html/permutation.html
vector<Point> *PointCloud::randomPoints(int numPts) const {
  const gsl_rng_type *T;
  gsl_rng *r;
  gsl_permutation *p;
  vector<Point> *randomPts = new vector<Point>(numPts);
  const vector<Point> allPts = getPoints();

  // Initialize random number generator
  gsl_rng_env_setup();
  r = gsl_rng_alloc(gsl_rng_default);

  // Use current time in nanoseconds as the seed
  struct timespec tm;
  clock_gettime(CLOCK_REALTIME, &tm);
  gsl_rng_set(r, tm.tv_nsec);

  // Initialize permutation
  p = gsl_permutation_alloc(n);
  gsl_permutation_init(p);

  // Create random permutation
  gsl_ran_shuffle(r, p->data, n, sizeof(size_t));

  // Use values from random permutation to select numPts
  for (size_t i = 0; i < numPts; i++) {
    (*randomPts)[i] = (allPts[p->data[i]]);
  }

  // Cleanup
  gsl_permutation_free(p);
  gsl_rng_free(r);

  return randomPts;
}
