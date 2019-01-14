// Include file for the photon map render code
#include "photon.h"

R2Image *RenderImage(R3Scene *scene, PhotonMap *global_photon_map,
  PhotonMap *caustic_photon_map, int num_nearest_photons,
  int width, int height, int print_verbose);
