/* kdtree.h
 * Author: Austin Le
 * Simple implementation of a k-dimensional tree storing Point objects.
*/

#ifndef KDTREE_H
#define KDTREE_H

#include <cstdlib>

#include "point.h"

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
  KDTreeNode *insert_(Point& p, size_t level, KDTreeNode *node);
  KDTreeNode *insert(Point& p);

  // Return nearest 3-dimensional point to v
  void nearestPoint_(const Point& p, size_t level, KDTreeNode *node,
    KDTreeNode *best, double *best_dist) const;
  Point nearestPoint(const Point& p) const;

  // Variables
  KDTreeNode *root;
};

#endif
