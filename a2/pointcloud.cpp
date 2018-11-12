#include <fstream>
#include <string>
#include <sstream>

#include "pointcloud.h"
#include "matrix4x4.h"
#include "vector4D.h"

using namespace std;

string Point::toString(void) const {
  stringstream ss;
  ss << cx << " " << cy << " " << cz << " " << nx << " " << ny << " " << nz;
  return ss.str();
}

Point Point::transform(const Matrix4x4& m) const {
  Vector4D v(cx, cy, cz, 1.0);
  Vector4D t = m * v;
  Vector4D n(nx, ny, nz, 1.0);
  Vector4D nt = m * n;

  return Point(t.x, t.y, t.z, nt.x, nt.y, nt.z);
}

void PointCloud::loadPointCloud(const char *filename) {
  double cx, cy, cz, nx, ny, nz;
  ifstream infile;
  string sfilename(filename);
  if (sfilename.compare(sfilename.size() - 4, 4, ".pts") != 0) {
    throw std::invalid_argument("File must be in .pts format");
  }

  infile.open(filename);

  n = 0;

  if (infile.is_open()) {
    // Read a line of 6 doubles
    while (infile >> cx >> cy >> cz >> nx >> ny >> nz) {
      points.push_back(Point(cx, cy, cz, nx, ny, nz));
      n++;
    }
    infile.close();
  }
}
