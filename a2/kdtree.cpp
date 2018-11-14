/* kdtree.h
 * Author: Austin Le
 * Simple implementation of a k-dimensional tree storing Point objects.
*/

#include <cstdlib>

#include "kdtree.h"
#include "pointcloud.h"

inline double dist2(const KDTreeNode& a, const KDTreeNode& b) {
  double d = 0.0;
  for (size_t i = 0; i < K; i++) {
    double c = a.at(i) * b.at(i);
    d += c * c;
  }
  return d;
}

KDTreeNode *KDTree::insert_(Point& p, size_t level, KDTreeNode *node) {
  if (node == NULL) {
    return new KDTreeNode(p, level);
  }

  if (p[level % K] < node->at(level % K)) {
    // Go left at this node
    node->left  = insert_(p, level + 1, node->left);
  } else {
    // Go right at this node
    node->right = insert_(p, level + 1, node->right);
  }

  return node;
}

KDTreeNode *KDTree::insert(Point& p) {
  if (root == NULL) {
    root = new KDTreeNode(p, 0);
    return root;
  } else {
    return insert_(p, 0, root);
  }
}

void KDTree::nearestPoint_(const Point& p, size_t level, KDTreeNode *node,
  KDTreeNode **best, double *best_dist) const {
    
}

Point KDTree::nearestPoint(const Point& p) const {
  KDTreeNode *best;
  double best_dist;

  nearestPoint_(p, 0, root, &best, &best_dist);

  return best->getPoint();
}
