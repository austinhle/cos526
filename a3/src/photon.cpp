#include "R3Graphics/R3Graphics.h"
#include "photon.h"

static R3Point
GetPhotonPosition(Photon *photon, void *dummy)
{
  R3Point pos = photon->position;
  return R3Point(pos.X(), pos.Y(), pos.Z());
}

int PhotonMap::BuildKdTree(void) {
  tree = new R3Kdtree<Photon>(intersections, GetPhotonPosition);
  if (!tree) {
    fprintf(stderr, "Unable to create KD tree\n");
    return 0;
  }

  return 1;
}

void PhotonMap::AddPhotonIntersection(Photon photon) {
  intersections.Insert(photon);
}
