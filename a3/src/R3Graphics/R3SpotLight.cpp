/* Source file for the R3 light class */



/* Include files */
#include <cmath>

#include "R3Graphics.h"



/* Public variables */

R3SpotLight R3null_spot_light;



/* Class type definitions */

RN_CLASS_TYPE_DEFINITIONS(R3SpotLight);



/* Public functions */

int
R3InitSpotLight()
{
    /* Return success */
    return TRUE;
}



void
R3StopSpotLight()
{
}



R3SpotLight::
R3SpotLight(void)
{
}



R3SpotLight::
R3SpotLight(const R3SpotLight& light)
    : R3PointLight(light),
      direction(light.direction),
      dropoffrate(light.dropoffrate),
      cutoffangle(light.cutoffangle)
{
    // Make sure direction is normalized
    this->direction.Normalize();
}



R3SpotLight::
R3SpotLight(const R3Point& position, const R3Vector& direction, const RNRgb& color,
	    RNScalar dropoffrate, RNAngle cutoffangle,
	    RNScalar intensity, RNBoolean active,
            RNScalar ca, RNScalar la, RNScalar qa)
    : R3PointLight(position, color, intensity, active, ca, la, qa),
      direction(direction),
      dropoffrate(dropoffrate),
      cutoffangle(cutoffangle)
{
    // Make sure direction is normalized
    this->direction.Normalize();
}



void R3SpotLight::
SetDirection(const R3Vector& direction)
{
    // Set direction
    this->direction = direction;
    this->direction.Normalize();
}



void R3SpotLight::
SetDropOffRate(RNScalar dropoffrate)
{
    // Set drop off rate
    this->dropoffrate = dropoffrate;
}



void R3SpotLight::
SetCutOffAngle(RNAngle cutoffangle)
{
    // Set cut off angle
    this->cutoffangle = cutoffangle;
}



RNScalar R3SpotLight::
IntensityAtPoint(const R3Point& point) const
{
    // Return intensity at point
    RNScalar I = R3PointLight::IntensityAtPoint(point);
    R3Vector ML = point - Position();
    ML.Normalize();
    RNScalar cos_alpha = ML.Dot(Direction());
    if (cos(cutoffangle) > cos_alpha) return 0.0;
    else return (I * pow(cos_alpha, dropoffrate));
}

R3Ray R3SpotLight::
GetPhotonRay(void) const
{
  R3Point position = Position();
  R3Vector sample_dir;
  RNScalar cos_alpha;

  // Use rejection sampling to get a sampled direction within the cutoff angle
  do {
    RNScalar e1 = RNRandomScalar();
    RNScalar e2 = RNRandomScalar();
    double x = cos(2.0 * RN_PI * e2) * sqrt(1.0 - pow(1.0 - e1, 2));
    double y = sin(2.0 * RN_PI * e2) * sqrt(1.0 - pow(1.0 - e1, 2));
    double z = 1.0 - e1;
    sample_dir = R3Vector(x, y, z);
    sample_dir.Normalize();

    // Rotate sampled direction into proper coordinate space
    R3Vector base = R3Vector(0.0, 0.0, 1.0);

    R3Vector normal = Direction();
    normal.Normalize();

    // Compute the axis of rotation
    R3Vector rotation_axis = base;
    rotation_axis.Cross(normal);

    // Compute number of radians to rotate by
    RNAngle angle = acos(base.Dot(normal));

    // Rotate the sample (by mutation)
    sample_dir.Rotate(rotation_axis, angle);

    // Determine angle between sampled direction and this direction
    cos_alpha = sample_dir.Dot(Direction());
  } while (cos(cutoffangle) > cos_alpha);

  return R3Ray(position, sample_dir);
}

void R3SpotLight::
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
    buffer[0] = Position().X();
    buffer[1] = Position().Y();
    buffer[2] = Position().Z();
    buffer[3] = 1.0;
    glLightfv(index, GL_POSITION, buffer);
    buffer[0] = Direction().X();
    buffer[1] = Direction().Y();
    buffer[2] = Direction().Z();
    glLightfv(index, GL_SPOT_DIRECTION, buffer);
    buffer[0] = DropOffRate();
    glLightf(index, GL_SPOT_EXPONENT, buffer[0]);
    buffer[0] = RN_RAD2DEG(CutOffAngle());
    glLightf(index, GL_SPOT_CUTOFF, buffer[0]);
    buffer[0] = ConstantAttenuation();
    buffer[1] = LinearAttenuation();
    buffer[2] = QuadraticAttenuation();
    glLightf(index, GL_CONSTANT_ATTENUATION, buffer[0]);
    glLightf(index, GL_LINEAR_ATTENUATION, buffer[1]);
    glLightf(index, GL_QUADRATIC_ATTENUATION, buffer[2]);
    glEnable(index);
}
