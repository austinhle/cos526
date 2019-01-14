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
  Photon(R3Point start_, R3Vector direction_, RNRgb power_) :
    position(start_), direction(direction_), power(power_), s_or_t(FALSE),
    bounces(0), start_pos(start_) {}
  Photon(R3Point start_, R3Vector direction_, RNRgb power_, RNBoolean s_or_t_) :
    position(start_), direction(direction_), power(power_), s_or_t(s_or_t_),
    bounces(0), start_pos(start_) {}

  // Property functions
  R3Ray Ray(void) const;

  R3Point position;   // incident position
  R3Vector direction; // incident direction
  RNRgb power;        // color (power)
  RNBoolean s_or_t;   // started with specular reflection or tramission?

  // Debugging properties
  int bounces;
  R3Point start_pos;
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
