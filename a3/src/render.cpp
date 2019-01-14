// Source file for photonmap renderer
// This implementation is a simple raycaster.
// Replace it with your own code.



////////////////////////////////////////////////////////////////////////
// Include files
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <algorithm>

#include "R3Graphics/R3Graphics.h"
#include "photon.h"

////////////////////////////////////////////////////////////////////////
// Function to render image with photon mapping
////////////////////////////////////////////////////////////////////////

static const RNScalar LOW = 0.0;
static const RNScalar HIGH = 1.0;

static RNScalar clamp(RNScalar value, RNScalar low, RNScalar high)
{
  return std::max(low, std::min(value, high));
}

static void clampColor(RNRgb *color)
{
  RNScalar r = color->R();
  RNScalar g = color->G();
  RNScalar b = color->B();

  color->Reset(clamp(r, LOW, HIGH), clamp(g, LOW, HIGH), clamp(b, LOW, HIGH));
}

static int ShadowRay(R3Scene *scene, R3SceneElement *hit_elem,
  R3Point pt, R3Light *light)
{
  // Local variables
  R3SceneNode *node;
  R3SceneElement *element;
  R3Shape *shape;
  R3Point point;
  R3Vector normal;
  RNScalar t;

  // Shadow ray variables
  R3Vector direction;
  R3Point origin;

  // Scene properties
  double radius = scene->BBox().DiagonalRadius();
  R3Point centroid = scene->BBox().Centroid();

  if (light->ClassID() == R3DirectionalLight::CLASS_ID()) {
    R3DirectionalLight *directional_light = (R3DirectionalLight *) light;
    direction = directional_light->Direction();
    origin = centroid - 1.25 * radius * direction;
  }
  else if (light->ClassID() == R3PointLight::CLASS_ID()) {
    R3PointLight *point_light = (R3PointLight *) light;
    origin = point_light->Position();
    direction = pt - origin;
  }
  else if (light->ClassID() == R3SpotLight::CLASS_ID()) {
    R3SpotLight *spot_light = (R3SpotLight *) light;
    origin = spot_light->Position();
    direction = pt - origin;
  }
  else if (light->ClassID() == R3AreaLight::CLASS_ID()) {
    R3AreaLight *area_light = (R3AreaLight *) light;
    origin = area_light->SamplePoint();
    direction = pt - origin;
  }
  else {
    std::cerr << "Unrecognized light ID" << std::endl;
    exit(-1);
  }

  R3Ray ray = R3Ray(origin + 0.05 * direction, direction);
  if (scene->Intersects(ray, &node, &element, &shape, &point, &normal, &t)) {
    if (element == hit_elem) {
      return 0;
    } else {
      return 1;
    }
  } else {
    return 0;
  }
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

            if (!ShadowRay(scene, element, point, light)) {
              direct += light->Reflection(*brdf, eye, point, normal);
            }
          }
          color += direct;

          // Compute indirect lighting using global_photon_map
          RNRgb indirect_global = RNblack_rgb;
          if (num_nearest_photons > 0) {
            // Find the nearest k photons to intersection point
            RNArray<Photon *> nearest_photons;
            RNLength distances[num_nearest_photons];
            global_photon_map->Tree()->FindClosest(point, 0, FLT_MAX,
                num_nearest_photons, nearest_photons, distances);

            for (int i = 0; i < nearest_photons.NEntries(); i++) {
              Photon *p = nearest_photons.Kth(i);
              indirect_global += p->power;
            }

            // Sum up photon power and divide by approximated sphere radius
            RNLength radius = distances[nearest_photons.NEntries() - 1];
            indirect_global /= (RN_PI * radius * radius);

            // Add indirect contribution to pixel color
            color += indirect_global;
          }

          // Add caustics using caustic_photon_map
          RNRgb caustic = RNblack_rgb;
          if (num_nearest_photons > 0) {
            // Find the nearest k photons to intersection point
            RNArray<Photon *> nearest_photons;
            RNLength distances[num_nearest_photons];
            caustic_photon_map->Tree()->FindClosest(point, 0, FLT_MAX,
                num_nearest_photons, nearest_photons, distances);

            for (int i = 0; i < nearest_photons.NEntries(); i++) {
              Photon *p = nearest_photons.Kth(i);
              caustic += p->power;
            }

            // Sum up photon power and divide by approximated sphere radius
            RNLength radius = distances[nearest_photons.NEntries() - 1];
            caustic /= (RN_PI * radius * radius);

            // Add caustic contribution to pixel color
            color += caustic;
          }
        }

        // Set pixel color
        clampColor(&color);
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
