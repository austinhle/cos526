/* kdtree.cpp
 * Author: Austin Le
 * Simple implementation of a k-dimensional tree storing Point objects.
*/

#include <cstdlib>

#include "kdtree.h"
#include "point.h"
#include "pointcloud.h"

namespace icp {

static double dist2(const KDTreeNode& a, const KDTreeNode& b) {
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

  if (node == NULL) return;

  double d = p.distanceTo(node->getPoint());
  double d2 = d * d; // Squared distance
  if (d2 < *best_dist) {
    *best_dist = d2;
    *best = node;
  }

  double diff = node->at(level % K) - p[level % K];
  double diff2 = diff * diff;

  // Explore further to left or right child
  nearestPoint_(p, level + 1, (diff > 0.0) ? node->left : node->right,
    best, best_dist);

  // If we didn't find better down that path, then also search the other path
  if (diff2 < *best_dist) {
    nearestPoint_(p, level + 1, (diff > 0.0) ? node->right : node->left,
    best, best_dist);
  }
}

Point KDTree::nearestPoint(const Point& p) const {
  KDTreeNode *best = root;
  double start_dist = p.distanceTo(root->getPoint());
  double best_dist = start_dist * start_dist;

  nearestPoint_(p, 0, root, &best, &best_dist);

  return best->getPoint();
}

} // namespace icp
