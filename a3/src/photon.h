// Include file for the photon and photon map data structures

#ifndef PHOTON_H
#define PHOTON_H

enum RR {
  DIFFUSE_REFLECTION,
  SPECULAR_REFLECTION,
  TRANSMISSION,
  ABSORPTION
};

class Photon {
public:
  Photon(void) {}
  Photon(R3Vector direction_, RNRgb power_) :
    direction(direction_), power(power_), s_or_t(FALSE) {}
  Photon(R3Point start_, R3Vector direction_, RNRgb power_) :
    position(start_), direction(direction_), power(power_), s_or_t(FALSE) {}

  // Property functions
  R3Ray Ray(void) const;

  R3Point position;   // incident position
  R3Vector direction; // incident direction
  RNRgb power;        // color (power)
  RNBoolean s_or_t;   // been through specular reflection or tramission?
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
