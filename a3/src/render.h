// Include file for the photon map render code
#ifndef RENDER_H
#define RENDER_H

#include "photon.h"

R2Image *RenderImage(R3Scene *scene, PhotonMap *global_photon_map,
  PhotonMap *caustic_photon_map, int num_nearest_photons,
  int specular_exponent, int num_samples, int width, int height,
  int print_verbose);

#endif
