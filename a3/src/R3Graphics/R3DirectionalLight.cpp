/* Source file for the R3 light class */



/* Include files */

#include "R3Graphics.h"



/* Public variables */

R3DirectionalLight R3null_directional_light;
R3DirectionalLight R3default_directional_light(R3Vector(0.0, 0.0, -1.0), RNRgb(1.0, 1.0, 1.0), 1.0, TRUE);



/* Class type definitions */

RN_CLASS_TYPE_DEFINITIONS(R3DirectionalLight);



/* Public functions */

int
R3InitDirectionalLight()
{
    /* Return success */
    return TRUE;
}



void
R3StopDirectionalLight()
{
}



R3DirectionalLight::
R3DirectionalLight(void)
{
}



R3DirectionalLight::
R3DirectionalLight(const R3DirectionalLight& light)
    : R3Light(light),
      direction(light.direction)
{
}



R3DirectionalLight::
R3DirectionalLight(const R3Vector& direction,
		   const RNRgb& color,
		   RNScalar intensity,
		   RNBoolean active)
    : R3Light(color, intensity, active),
      direction(direction)
{
    // Make sure direction is normalized
    this->direction.Normalize();
}



void R3DirectionalLight::
SetDirection(const R3Vector& direction)
{
    // Set direction
    this->direction = direction;
    this->direction.Normalize();
}



RNRgb R3DirectionalLight::
DiffuseReflection(const R3Brdf& brdf,
    const R3Point& point, const R3Vector& normal) const
{
    // Check if light is active
    if (!IsActive()) return RNblack_rgb;

    // Get material properties
    const RNRgb& Dc = brdf.Diffuse();

    // Get light properties
    const RNRgb& Ic = Color();
    RNScalar I = Intensity();
    R3Vector L = -(Direction());

    // Compute geometric stuff
    RNScalar NL = normal.Dot(L);
    if (RNIsNegativeOrZero(NL)) return RNblack_rgb;

    // Return diffuse component of reflection
    return (I * NL) * Dc * Ic;
}



RNRgb R3DirectionalLight::
SpecularReflection(const R3Brdf& brdf, const R3Point& eye,
    const R3Point& point, const R3Vector& normal) const
{
    // Check if light is active
    if (!IsActive()) return RNblack_rgb;

    // Get material properties
    const RNRgb& Sc = brdf.Specular();
    RNScalar s = brdf.Shininess();

    // Get light properties
    const RNRgb& Ic = Color();
    RNScalar I = Intensity();
    R3Vector L = -(Direction());

    // Compute geometric stuff
    RNScalar NL = normal.Dot(L);
    if (RNIsNegativeOrZero(NL)) return RNblack_rgb;
    R3Vector R = (2.0 * NL) * normal - L;
    R3Vector V = eye - point;
    V.Normalize();
    RNScalar VR = V.Dot(R);
    if (RNIsNegativeOrZero(VR)) return RNblack_rgb;

    // Return specular component of reflection
    return (I * pow(VR,s)) * Sc * Ic;
}



RNRgb R3DirectionalLight::
Reflection(const R3Brdf& brdf, const R3Point& eye,
    const R3Point& point, const R3Vector& normal) const
{
    // Check if light is active
    if (!IsActive()) return RNblack_rgb;

    // Get material properties
    const RNRgb& Dc = brdf.Diffuse();
    const RNRgb& Sc = brdf.Specular();
    RNScalar s = brdf.Shininess();

    // Get light properties
    RNScalar I = Intensity();
    R3Vector L = -(Direction());
    const RNRgb& Ic = Color();

    // Compute geometric stuff
    RNScalar NL = normal.Dot(L);
    if (RNIsNegativeOrZero(NL)) return RNblack_rgb;
    R3Vector R = (2.0 * NL) * normal - L;
    R3Vector V = eye - point;
    V.Normalize();
    RNScalar VR = V.Dot(R);

    // Compute diffuse reflection
    RNRgb rgb = (I * NL) * Dc * Ic;

    // Compute specular reflection
    if (RNIsPositive(VR)) rgb += (I * pow(VR,s)) * Sc * Ic;

    // Return total reflection
    return rgb;
}

R3Ray R3DirectionalLight::
GetPhotonRay(void) const
{
  // Scene properties
  R3Scene *scene = Scene();
  double radius = scene->BBox().DiagonalRadius();
  R3Point centroid = scene->BBox().Centroid();

  // Compute a location outside of the scene
  R3Point pos = centroid - 1.25 * radius * direction;

  // Create a circle
  R3Circle circle = R3Circle(pos, radius, direction);
  R3Vector dir = circle.Normal();
  RNDimension dim = dir.MinDimension();
  R3Vector axis1 = dir % R3xyz_triad[dim];
  axis1.Normalize();
  R3Vector axis2 = dir % axis1;
  axis2.Normalize();

  // Sample a point on that circle
  RNScalar r1, r2;
  do {
    r1 = RNRandomScalar();
    r2 = RNRandomScalar();
  } while (r1*r1 + r2*r2 > 1);

  R3Point sample_point = pos;
  sample_point += r1 * radius * axis1;
  sample_point += r2 * radius * axis2;

  return R3Ray(sample_point, direction);
}

void R3DirectionalLight::
Draw(int i) const
{
    // Draw light
    GLenum index = (GLenum) (GL_LIGHT2 + i);
    if (index > GL_LIGHT7) return;
    GLfloat buffer[4];
    buffer[0] = Intensity() * Color().R();
    buffer[1] = Intensity() * Color().G();
    buffer[2] = Intensity() * Color().B();
    buffer[3] = 1.0;
    glLightfv(index, GL_DIFFUSE, buffer);
    glLightfv(index, GL_SPECULAR, buffer);
    buffer[0] = -(Direction().X());
    buffer[1] = -(Direction().Y());
    buffer[2] = -(Direction().Z());
    buffer[3] = 0.0;
    glLightfv(index, GL_POSITION, buffer);
    buffer[0] = 180.0;
    glLightf(index, GL_SPOT_CUTOFF, buffer[0]);
    glEnable(index);
}
