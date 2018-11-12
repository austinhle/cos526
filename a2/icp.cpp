#include <fstream>
#include <iostream>
#include <string>

#include "pointcloud.h"
#include "vector3D.h"
#include "matrix4x4.h"
#include "vector4D.h"
#include "matrix6x6.h"
#include "vector6D.h"

using namespace std;

// TODO: Implement 6x6 linear solver
static void solve(void) {

}

static void loadData(const char* c1, const char* c2,
  PointCloud* pc1, PointCloud* pc2, Matrix4x4* m1, Matrix4x4* m2) {

  // Put together all relevant file names
  string file1(c1);
  string file2(c2);
  string filename1 = file1.substr(file1.size() - 4);
  string filename2 = file2.substr(file2.size() - 4);
  string xfname1 = filename1 + "o.xf";
  string xfname2 = filename2 + "o.xf";

  // Load point clouds
  pc1->loadPointCloud(file1.c_str());
  pc2->loadPointCloud(file2.c_str());

  // Load transformation matrices
  ifstream xf1(xfname1);
  if (xf1.good()) {
    m1->loadMatrix(xfname1.c_str());
  } else {
    *m1 = Matrix4x4::identity();
  }

  ifstream xf2(xfname2);
  if (xf2.good()) {
    m2->loadMatrix(xfname2.c_str());
  } else {
    *m2 = Matrix4x4::identity();
  }
}

int main(int argc, const char *argv[]) {
  // 1. Read in the point clouds and transformations.
  PointCloud pc1, pc2;
  Matrix4x4 m1, m2;
  loadData(argv[1], argv[2], &pc1, &pc2, &m1, &m2);

  // 2. Randomly pick 1000 points from PC1.

  // 3. For each point in PC1 chosen in (2), apply M1 and then inverse of M2,
  // producing the point's coordinates in PC2's coordinate system, called p_i.

    // 4. Find the closest point in PC2, called q_i. Each q_i has normal n_i.
    // DEBUG HERE!

  // 5. For each triplet (p_i, q_i, n_i), compute the median point-to-plane
  // distance. There are 1000 triplets in total.

  // 6. Perform outlier rejection. Eliminate point-pairs whose point-to-plane
  // distance is more than 3x the median from (5).

  // 7. Compute new median point-to-plane distance of remaining point-pairs.

  // 8. For each point i, construct matrix C_i and vector D_i.
  // Sum up all matrices C_i into a matrix C.
  // Sum up all vectors D_i into a vector D.

  // 9. Solve Cx = d.
  // x is a 6x1 vector containing (Rx, Ry, Rz, Tx, Ty, Tz).
  // Construct M_icp from T*Rz*Ry*Rx.

  // 10. Create new M1' = M2*M_icp*M2i*M1.

  // 11. Update all p_i from (6) by transforming them by M_icp.
  // Recompute median point-to-plane distance.

  // 12. If ratio of distances in (11) to (7) is less than 0.999, continue.

  // 13. Write out M1' to file1.xf.

  return 0;
}
