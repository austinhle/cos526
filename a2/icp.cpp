#include <fstream>
#include <iostream>
#include <string>

#include <gsl/gsl_linalg.h>

#include "pointcloud.h"
#include "vector3D.h"
#include "matrix4x4.h"
#include "vector4D.h"
#include "matrix6x6.h"
#include "vector6D.h"

using namespace std;

// Solve Ax = b. x will contain the solution.
static void solve(Matrix6x6& A, Vector6D& b, Vector6D& x) {
  double *A_data = (double *) &A;
  double *b_data = (double *) &b;

  // Create GSL structures using the data
  gsl_matrix_view A_m = gsl_matrix_view_array(A_data, 6, 6);
  gsl_vector_view b_v = gsl_vector_view_array(b_data, 6);
  gsl_vector *res = gsl_vector_alloc(6);

  // Solve the system using LU-decomposition of A
  int s;
  gsl_permutation *p = gsl_permutation_alloc(6);
  gsl_linalg_LU_decomp (&A_m.matrix, p, &s);
  gsl_linalg_LU_solve (&A_m.matrix, p, &b_v.vector, res);

  // Debugging
  printf ("res = \n");
  gsl_vector_fprintf (stdout, res, "%g");

  // Extract values from res into x
  for (int i = 0; i < 6; i++) {
    x[i] = gsl_vector_get(res, i);
  }

  // Cleanup
  gsl_permutation_free(p);
  gsl_vector_free(res);
}

static void loadData(const char* c1, const char* c2,
  PointCloud& pc1, PointCloud& pc2, Matrix4x4* m1, Matrix4x4* m2) {

  // Put together all relevant file names
  string file1(c1);
  string file2(c2);
  string filename1 = file1.substr(file1.size() - 4);
  string filename2 = file2.substr(file2.size() - 4);
  string xfname1 = filename1 + "o.xf";
  string xfname2 = filename2 + "o.xf";

  cout << "Loading point clouds..." << endl;

  // Load point clouds
  pc1.loadPointCloud(file1.c_str());
  pc2.loadPointCloud(file2.c_str());

  cout << "Done loading point clouds!" << endl;
  cout << "Loading transformation matrices..." << endl;

  // Load transformation matrices
  ifstream xf1(xfname1);
  if (xf1.good()) {
    m1->loadMatrix(xfname1.c_str());
    cout << "Loaded matrix 1!" << endl;
  } else {
    *m1 = Matrix4x4::identity();
    cout << "Could not find matrix 1; using identity instead!" << endl;
  }

  ifstream xf2(xfname2);
  if (xf2.good()) {
    m2->loadMatrix(xfname2.c_str());
    cout << "Loaded matrix 2!" << endl;
  } else {
    *m2 = Matrix4x4::identity();
    cout << "Could not find matrix 2; using identity instead!" << endl;
  }
}

int main(int argc, const char *argv[]) {
  // 1. Read in the point clouds and transformations.
  PointCloud pc1, pc2;
  Matrix4x4 m1, m2;
  loadData(argv[1], argv[2], pc1, pc2, &m1, &m2);

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
