// Source file for photonmap renderer
// This implementation is a simple raycaster.
// Replace it with your own code.



////////////////////////////////////////////////////////////////////////
// Include files
////////////////////////////////////////////////////////////////////////

#include <algorithm>

#include "R3Graphics/R3Graphics.h"
#include "photon.h"

////////////////////////////////////////////////////////////////////////
// Function to render image with photon mapping
////////////////////////////////////////////////////////////////////////

static RNScalar clamp(RNScalar value, RNScalar low, RNScalar high)
{
  return std::max(low, std::min(value, high));
}

R2Image *
RenderImage(R3Scene *scene,
  PhotonMap *global_photon_map,
  PhotonMap *caustic_photon_map,
  int num_nearest_photons,
  int width, int height,
  int print_verbose)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();
  int ray_count = 0;

  // Allocate image
  R2Image *image = new R2Image(width, height);
  if (!image) {
    fprintf(stderr, "Unable to allocate image\n");
    return NULL;
  }

  // Convenient variables
  const R3Point& eye = scene->Camera().Origin();
  R3SceneNode *node;
  R3SceneElement *element;
  R3Shape *shape;
  R3Point point;
  R3Vector normal;
  RNScalar t;

  // Draw intersection point and normal for some rays
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      R3Ray ray = scene->Viewer().WorldRay(i, j);
      if (scene->Intersects(ray, &node, &element, &shape, &point, &normal, &t)) {
        // Get intersection information
        const R3Material *material = (element) ? element->Material() : &R3default_material;
        const R3Brdf *brdf = (material) ? material->Brdf() : &R3default_brdf;

        // Compute color
        RNRgb color = scene->Ambient();
        if (brdf) {
          color += brdf->Emission();

          // Compute direct lighting from each light
          RNRgb direct = RNblack_rgb;
          for (int k = 0; k < scene->NLights(); k++) {
            R3Light *light = scene->Light(k);
            direct += light->Reflection(*brdf, eye, point, normal);
          }
          color += direct;

          // Compute indirect lighting using photon map, if requested
          RNRgb indirect = RNblack_rgb;
          if (num_nearest_photons > 0) {
            // Find the nearest k photons to intersection point
            RNArray<Photon *> nearest_photons;
            RNLength distances[num_nearest_photons];
            global_photon_map->Tree()->FindClosest(point, 0, FLT_MAX,
                num_nearest_photons, nearest_photons, distances);

            for (int i = 0; i < nearest_photons.NEntries(); i++) {
              Photon *p = nearest_photons.Kth(i);
              indirect += p->power;
            }

            // Sum up photon power and divide by approximated sphere radius
            RNLength radius = distances[nearest_photons.NEntries() - 1];
            indirect /= (RN_PI * radius * radius);

            // Add indirect contribution to pixel color
            color += indirect;
          }
        }

        // Set pixel color
        color = RNRgb(clamp(color.R(), 0.0, 1.0),
                      clamp(color.G(), 0.0, 1.0),
                      clamp(color.B(), 0.0, 1.0));
        image->SetPixelRGB(i, j, color);

        // Update ray count
        ray_count++;
      }
    }
  }

  // Print statistics
  if (print_verbose) {
    printf("Rendered image ...\n");
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  # Rays = %d\n", ray_count);
    fflush(stdout);
  }

  // Return image
  return image;
}
