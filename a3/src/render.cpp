// Source file for photonmap renderer
// This implementation is a simple raycaster.
// Replace it with your own code.



////////////////////////////////////////////////////////////////////////
// Include files
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <algorithm>
#include <cmath>

#include "R3Graphics/R3Graphics.h"
#include "render.h"

////////////////////////////////////////////////////////////////////////
// Function to render image with photon mapping
////////////////////////////////////////////////////////////////////////

static const RNScalar LOW = 0.0;
static const RNScalar HIGH = 1.0;

// Normal vector of coordinate system used to sample vectors
static const R3Vector BASE = R3Vector(0.0, 0.0, 1.0);

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

// Rotate sample to the coordinate system defined by normal.
static void RotateTo(R3Vector& sample, R3Vector &normal)
{
  // Normalize the normal vector defining the new coordinate system
  normal.Normalize();

  // Compute the axis of rotation
  R3Vector rotation_axis = BASE;
  rotation_axis.Cross(normal);

  // Compute number of radians to rotate by
  RNAngle angle = acos(BASE.Dot(normal));

  // Rotate the sample (by mutation)
  sample.Rotate(rotation_axis, angle);
}

static RR RussianRoulette(const R3Brdf *brdf, RNRgb *brdf_val)
{
  double total_r = 0.0;
  double total_g = 0.0;
  double total_b = 0.0;

  // Compute probabilities
  double pd, ps, pt;
  if (brdf->IsDiffuse()) {
    const RNRgb diffuse = brdf->Diffuse();
    pd = std::max(std::max(diffuse.R(), diffuse.G()), diffuse.B());
    total_r += diffuse.R();
    total_g += diffuse.G();
    total_b += diffuse.B();
  } else {
    pd = 0.0;
  }

  if (brdf->IsSpecular()) {
    const RNRgb specular = brdf->Specular();
    ps = std::max(std::max(specular.R(), specular.G()), specular.B());
    total_r += specular.R();
    total_g += specular.G();
    total_b += specular.B();
  } else {
    ps = 0.0;
  }

  if (brdf->IsTransparent()) {
    const RNRgb transmission = brdf->Transmission();
    pt = std::max(std::max(transmission.R(), transmission.G()), transmission.B());
    total_r += transmission.R();
    total_g += transmission.G();
    total_b += transmission.B();
  } else {
    pt = 0.0;
  }

  // Normalize the probabilities if they exceed 1.0
  double total = pd + ps + pt;
  if (total > 1.0) {
    pd /= total; ps /= total; pt /= total;
  }

  // Perform Russian Roulette to determine which action to take next
  double k = RNRandomScalar();
  if (k < pd) {
    *brdf_val = brdf->Diffuse();
    brdf_val->SetRed(brdf_val->R() / (pd * total_r));
    brdf_val->SetGreen(brdf_val->G() / (pd * total_g));
    brdf_val->SetBlue(brdf_val->B() / (pd * total_b));
    return DIFFUSE_REFLECTION;
  } else if (k < pd + ps) {
    *brdf_val = brdf->Specular();
    brdf_val->SetRed(brdf_val->R() / (ps * total_r));
    brdf_val->SetGreen(brdf_val->G() / (ps * total_g));
    brdf_val->SetBlue(brdf_val->B() / (ps * total_b));
    return SPECULAR_REFLECTION;
  } else if (k < pd + ps + pt) {
    *brdf_val = brdf->Transmission();
    brdf_val->SetRed(brdf_val->R() / (pt * total_r));
    brdf_val->SetGreen(brdf_val->G() / (pt * total_g));
    brdf_val->SetBlue(brdf_val->B() / (pt * total_b));
    return TRANSMISSION;
  } else {
    return ABSORPTION;
  }
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

static RNRgb
EstimateDirect(R3Scene *scene, R3SceneElement *element,
  R3Point point, const R3Brdf *brdf, R3Point eye, R3Vector normal)
{
  RNRgb direct = RNblack_rgb;
  for (int k = 0; k < scene->NLights(); k++) {
    R3Light *light = scene->Light(k);

    if (!ShadowRay(scene, element, point, light)) {
      direct += light->Reflection(*brdf, eye, point, normal);
    }
  }

  return direct;
}

static RNRgb
EstimateIndirect(PhotonMap *global_photon_map, R3Point point,
  int num_nearest_photons)
{
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
    double radius = distances[nearest_photons.NEntries() - 1];
    double area = 1.0 * RN_PI * radius * radius;
    indirect /= area;
  }

  return indirect;
}

static RNRgb
EstimateCaustic(PhotonMap *caustic_photon_map, R3Point point,
  int num_nearest_photons)
{
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
    double radius = distances[nearest_photons.NEntries() - 1];
    double area = 1.0 * RN_PI * radius * radius;
    caustic /= area;
  }

  return caustic;
}

static RNRgb
TraceRay(R3Scene *scene, PhotonMap *global_photon_map,
  PhotonMap *caustic_photon_map,
  int num_nearest_photons, int specular_exponent, R3Ray ray, int depth,
  int *ray_count)
{
  // Local variables
  const R3Point& eye = scene->Camera().Origin();
  R3SceneNode *node;
  R3SceneElement *element;
  R3Shape *shape;
  R3Point point;
  R3Vector normal;
  RNScalar t;

  // Increment ray count
  (*ray_count)++;

  // Initial color
  RNRgb color = RNblack_rgb;

  if (scene->Intersects(ray, &node, &element, &shape, &point, &normal, &t)) {
    // Get intersection information
    const R3Material *material = (element) ? element->Material() : &R3default_material;
    const R3Brdf *brdf = (material) ? material->Brdf() : &R3default_brdf;

    // Get light vector
    R3Vector l = ray.Vector();
    l.Normalize();

    // Get normal vector
    R3Vector n = normal;
    n.Normalize();

    // Add ambient lighting
    color += scene->Ambient();

    // Add emission from intersecting material
    if (brdf) {
      color += brdf->Emission();
    }

    // Add direct lighting
    RNRgb direct = EstimateDirect(scene, element, point, brdf, eye, n);
    color += direct;

    // Add indirect lighting
    RNRgb indirect = EstimateIndirect(global_photon_map, point, num_nearest_photons);
    color += indirect * brdf->Diffuse();

    // Add caustics
    RNRgb caustics = EstimateCaustic(caustic_photon_map, point, num_nearest_photons);
    color += caustics * brdf->Diffuse();

    // Russian Roulette + recursive ray tracing for specular component
    RNRgb mc_color = RNblack_rgb;
    if (brdf) {
      RNRgb brdf_val;
      RR rr = RussianRoulette(brdf, &brdf_val);

      switch (rr) {
        case DIFFUSE_REFLECTION: {
          // Sample a diffuse reflection direction
          RNScalar u1 = RNRandomScalar();
          RNScalar u2 = RNRandomScalar();
          RNAngle pitch = 2.0 * RN_PI * u2;
          RNAngle yaw = acos(sqrt(u1));
          R3Vector dir = R3Vector(pitch, yaw);
          RotateTo(dir, n);

          // Create new secondary ray to trace
          R3Ray next_ray = R3Ray(point + 0.05 * dir, dir);

          // Get recursive ray-traced color
          mc_color = TraceRay(scene, global_photon_map, caustic_photon_map,
            num_nearest_photons, specular_exponent, next_ray, depth + 1, ray_count);
          mc_color = mc_color * brdf_val;
          // clampColor(&mc_color);

          color += mc_color;

          break;
        }
        case SPECULAR_REFLECTION: {
          // Sample a specular reflection direction
          RNScalar u1 = RNRandomScalar();
          RNScalar u2 = RNRandomScalar();
          RNAngle pitch = 2.0 * RN_PI * u2;
          RNAngle yaw = acos(pow(u1, 1.0 / (specular_exponent + 1.0)));
          R3Vector dir = R3Vector(pitch, yaw);
          RotateTo(dir, n);

          // Create new secondary ray to trace
          R3Ray next_ray = R3Ray(point + 0.05 * dir, dir);

          // Add recursive ray-traced color
          mc_color = TraceRay(scene, global_photon_map, caustic_photon_map,
            num_nearest_photons, specular_exponent, next_ray, depth + 1, ray_count);
          mc_color = mc_color * brdf_val;
          // clampColor(&mc_color);

          color += mc_color;

          break;
        }
        case TRANSMISSION: {
          RNScalar ior1 = 1.0; // incoming index of refraction
          RNScalar ior2 = 1.0; // outgoing index of refraction

          RNScalar c = -n.Dot(l);
          RNBoolean inside = (c < 0) ? TRUE : FALSE;

          if (inside == TRUE) { // Light is coming from inside the object
            n = -n;
            c = -n.Dot(l);
            ior1 = brdf->IndexOfRefraction();
          }
          else {
            ior2 = brdf->IndexOfRefraction();
          }

          RNScalar r = ior1 / ior2;
          RNScalar s2 = r * sqrt(1.0 - pow(c, 2));
          if (s2 > 1.0) { // Total internal reflection
            break; // Terminate the ray; treat as ABSORPTION case
          }
          else {
            // Compute refracted direction
            R3Vector dir = r * l + (r * c - sqrt(1 - pow(r, 2) * (1 - pow(c, 2)))) * n;

            // Create new secondary ray to trace
            R3Ray next_ray = R3Ray(point + 0.05 * dir, dir);

            // Add recursive ray-traced color
            mc_color = TraceRay(scene, global_photon_map, caustic_photon_map,
              num_nearest_photons, specular_exponent, next_ray, depth + 1, ray_count);
            mc_color = mc_color * brdf_val;
            // clampColor(&mc_color);

            color += mc_color;
          }

          break;
        }
        case ABSORPTION: {
          break;
        }
        default: {
          std::cerr << "Invalid Russian Roulette state while ray-tracing" << std::endl;
          exit(-1);
        }
      }
    }
  }

  clampColor(&color);
  return color;
}

R2Image *
RenderImage(R3Scene *scene,
  PhotonMap *global_photon_map,
  PhotonMap *caustic_photon_map,
  int num_nearest_photons,
  int specular_exponent,
  int num_samples,
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

  // Draw intersection point and normal for some rays
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      RNRgb color = RNblack_rgb;
      for (int k = 0; k < num_samples; k++) {
        R3Ray ray = scene->Viewer().WorldRay(i, j);
        color += TraceRay(scene, global_photon_map, caustic_photon_map,
          num_nearest_photons, specular_exponent, ray, 0, &ray_count);
      }
      color /= num_samples;
      clampColor(&color);

      // color *= 0.5;

      // Set pixel color
      image->SetPixelRGB(i, j, color);
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
