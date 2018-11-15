/* kdtree.h
 * Author: Austin Le
 * Interface for k-dimensional tree storing Point objects.
*/

#ifndef KDTREE_H
#define KDTREE_H

#include <cstdlib>

#include "point.h"

namespace icp {

const size_t K = 3;

class KDTreeNode {
public:
  // Constructors
  KDTreeNode(void) : level(0), left(NULL), right(NULL) {}
  KDTreeNode(Point p_, size_t level_) : p(p_), level(level_),
    left(NULL), right(NULL) {}
  KDTreeNode(Point p_, size_t level_, KDTreeNode *left_, KDTreeNode *right_) :
    p(p_), level(level_), left(left_), right(right_) {}

  // Getters
  inline const double at(size_t i) const { return p[i]; }
  inline const size_t getLevel(void) const { return level; }
  inline const Point& getPoint(void) const { return p; }

  // Variables
  KDTreeNode *left, *right;

private:
  size_t level;
  Point p;
};

class KDTree {
public:
  // Default constructor
  KDTree(void) : root(NULL) {}

  // Insert a 3-dimensional data point, return root of modified subtree
  KDTreeNode *insert(Point& p);

  // Return nearest 3-dimensional point to v
  Point nearestPoint(const Point& p) const;

  // Getters
  inline KDTreeNode *getRoot(void) { return root; }
  inline const KDTreeNode *getRoot(void) const { return root; }

private:
  KDTreeNode *insert_(Point& p, size_t level, KDTreeNode *node);

  void nearestPoint_(const Point& p, size_t level, KDTreeNode *node,
    KDTreeNode **best, double *best_dist) const;

  // Variables
  KDTreeNode *root;
};

} // namespace icp

#endif
