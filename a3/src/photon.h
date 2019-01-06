// Include file for the photon and photon map data structures

#ifndef PHOTON_H
#define PHOTON_H

struct Photon {
  R3Point position;   // position
  R3Vector direction; // incident direction
  RNRgb power;        // color (power)
};

class PhotonMap {
public:
  // Constructors
  PhotonMap(void) {}

  // Property functions
  const R3Kdtree<Photon *> *Tree(void) const { return tree; }
  const RNArray<Photon *>& Intersections(void) const { return intersections; }

  // Manipulation functions/operations
  int BuildKdTree(void);
  void AddPhotonIntersection(Photon *photon);

private:
  R3Kdtree<Photon *> *tree;
  RNArray<Photon *> intersections;
};

#endif
