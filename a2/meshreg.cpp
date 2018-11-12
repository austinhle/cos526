#include "pointcloud.h"
#include "matrix4x4.h"
#include "vector4D.h"

#include <iostream>

using namespace std;

int main(int argc, const char *argv[]) {
  PointCloud pc;
  pc.loadPointCloud("bun000.pts");
  const vector<Point>& points = pc.getPoints();
  Point p = points[0];
  cout << p.toString() << endl;

  return 0;
}
