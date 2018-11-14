#include <fstream>
#include <iostream>
#include <string>
#include <numeric>

#include <gsl/gsl_linalg.h>

#include "point.h"
#include "pointcloud.h"
#include "vector3D.h"
#include "matrix4x4.h"
#include "vector4D.h"
#include "matrix6x6.h"
#include "vector6D.h"

using namespace std;

const size_t N = 1000;
const double THRESHOLD = 0.9999;

// Return the median value of the given vector, vals.
static double median(vector<double>& vals) {
  size_t i1 = vals.size() / 2 - 1;
  size_t i2 = vals.size() / 2;

  nth_element(vals.begin(), vals.begin() + i1, vals.end());
  nth_element(vals.begin(), vals.begin() + i2, vals.end());

  return 0.5 * (vals[i1] + vals[i2]);
}

// Solve Ax = b. x will contain the solution.
// Adapted from https://www.gnu.org/software/gsl/doc/html/linalg.html#lu-decomposition
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
  string xfname1 = file1.substr(0, file1.size() - 4) + ".xf";
  string xfname2 = file2.substr(0, file2.size() - 4) + ".xf";

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
  /* (1) Read in the point clouds and transformations. */
  PointCloud pc1, pc2;
  Matrix4x4 m1, m2, m2_inv, m1_to_m2;
  loadData(argv[1], argv[2], pc1, pc2, &m1, &m2);
  m2_inv = m2.inverse();

  // Initialize KD-trees within point clouds
  pc1.initKDTree();
  pc2.initKDTree();

  // Variables to store mean point-to-plane distances
  // Used to determine convergence/stopping condition
  double valid_mean, new_mean;
  size_t iterations = 0;
  do {
    /* (2) Randomly pick 1000 points from pc1. */
    vector<Point> *randoms = pc1.randomPoints(N);

    // (2.1) Precompute transformation from m1's coordinate system to m2's
    m1_to_m2 = m2_inv * m1;

    /* (3) For each point in pc1 chosen in (2)... */
    vector<Point> ps, qs;
    for (const Point& rp : *randoms) {
      // (3.1) Apply m1 and then inverse of m2 to pt to create p_i.
      Point p_i = rp.transform(m1_to_m2);
      ps.push_back(p_i);

      /* (4) For this p_i, find the closest point in pc2, called q_i.
       * Each q_i has normal n_i within it. */
      Point q_i = pc2.nearestBruteForce(p_i);
      qs.push_back(q_i);
    }

    /* (5) For each pair, compute the median point-to-plane distance. */
    vector<double> dists;
    for (size_t i = 0; i < N; i++) {
      Vector3D piv, qiv, qin;
      double point_to_plane_dist;
      piv = ps[i].toCoordsVector3D();
      qiv = qs[i].toCoordsVector3D();
      qin = qs[i].toNormalVector3D();

      // Compute point-to-plane distance
      point_to_plane_dist = abs(dot(piv - qiv, qin));
      dists.push_back(point_to_plane_dist);
    }

    // Make a copy, since median is destructive
    vector<double> dists_copy = dists;

    // Compute the median point-to-plane distance from all N pairs
    double med = median(dists_copy);
    cout << "==DEBUG== Median of all N is: " << med << endl;

    /* (6) Perform outlier rejection. Eliminate point-pairs whose point-to-plane
     * distance is more than 3x the median from (5).
     */
    vector<bool> valid(N, true);
    for (size_t i = 0; i < N; i++) {
      if (dists[i] > 3.0 * med) {
        valid[i] = false;
      }
    }

    /* (7) Compute new mean point-to-plane distance of remaining point-pairs. */
    double total = 0.0;
    size_t count = 0;
    for (size_t i = 0; i < N; i++) {
      if (valid[i]) {
        total += dists[i];
        count++;
      }
    }
    valid_mean = total / count;
    cout << "==DEBUG== Mean of valid is: " << valid_mean << endl;

    /* (8) For each point i, construct matrix C_i and vector d_i.
     * Sum up all matrices C_i into a matrix C.
     * Sum up all vectors d_i into a vector d.
     */
    // Accumulated terms
    Matrix6x6 C;
    Vector6D d;
    for (size_t i = 0; i < N; i++) {
      if (valid[i]) {
        // Intermediate terms
        Matrix6x6 C_i;
        Vector6D A_i, d_i;
        Vector3D p_cross_n, piv, qiv, qin;
        double b_i;

        // Initialize vectors for computation
        piv = ps[i].toCoordsVector3D();
        qiv = qs[i].toCoordsVector3D();
        qin = qs[i].toNormalVector3D();

        // Construct A_i
        p_cross_n = cross(piv, qin);
        A_i[0] = p_cross_n.x;
        A_i[1] = p_cross_n.y;
        A_i[2] = p_cross_n.z;
        A_i[3] = qin.x;
        A_i[4] = qin.y;
        A_i[5] = qin.z;

        // Compute b_i
        b_i = -dot(piv - qiv, qin);

        // Compute d_i
        d_i = A_i * b_i;

        // Compute C_i
        C_i = outer(A_i, A_i);

        // Accumulate the terms
        C += C_i;
        d += d_i;
      }
    }

    /* (9) Solve Cx = d.
     * x is a 6x1 vector containing (Rx, Ry, Rz, Tx, Ty, Tz).
     * Construct M_icp from T*Rz*Ry*Rx.
     */
    Vector6D x;
    solve(C, d, x);
    Matrix4x4 r, t, m_icp;
    r = Matrix4x4::rotation(x[0], x[1], x[2]);
    t = Matrix4x4::translation(x[3], x[4], x[5]);
    m_icp = t * r;

    /* (10) Update: M1 = M2*M_icp*M2i*M1. */
    m1 = m2 * m_icp * m2_inv * m1;

    /* (11) Update all p_i from (6) by transforming them by M_icp.
     * Recompute mean point-to-plane distance.
     */
    total = 0.0;
    count = 0;
    for (size_t i = 0; i < N; i++) {
      Vector3D piv, qiv, qin;
      double point_to_plane_dist;
      if (valid[i]) {
        // Update all valid p_i with M_icp
        ps[i] = ps[i].transform(m_icp);

        // Extract vectors
        piv = ps[i].toCoordsVector3D();
        qiv = qs[i].toCoordsVector3D();
        qin = qs[i].toNormalVector3D();

        // Compute point-to-plane distance
        total += abs(dot(piv - qiv, qin));
        count++;
      }
    }
    new_mean = total / count;
    cout << "==DEBUG== Mean after update is: " << new_mean << endl;
    cout << "Ratio of new_mean / valid_mean is: " << new_mean / valid_mean << endl;

    iterations++;
    /* (12) If ratio of distances in (11) to (7) is less than 0.999, continue. */
  } while ((new_mean / valid_mean) < THRESHOLD);

  cout << "==DEBUG== ICP converged after " << iterations << " iterations" << endl;

  /* (13) Write out new M1 to file1.xf. */
  m1.saveMatrix(argv[1]);

  return 0;
}
