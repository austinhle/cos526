#include "imageblend.h"

void translate(Im *src, Im *dst, int x, int y)
{
  std::vector<Color> new_pixels;
  new_pixels.resize(dst->w() * dst->h());

  for (size_t i = 0; i < src->w(); i++) {
    for (size_t j = 0; j < src->h(); j++) {
      int new_x = (i+x >= dst->w()) ? dst->w() - 1 : i+x;
      int new_y = (j+y >= dst->h()) ? dst->h() - 1 : j+y;

      new_pixels[(new_x) + (new_y) * dst->w()] = (*src)(i, j);
    }
  }

  src->setWidth(dst->w());
  src->setHeight(dst->h());
  src->setPixels(new_pixels);
}

void non_seamless_clone(Im *src, Im *dst, Im *mask, Im *out)
{
  for (int i = 0; i < out->w(); i++)
    for (int j = 0; j < out->h(); j++)
      (*out)(i, j) = (*mask)(i, j).isWhite() ? (*src)(i, j) : (*dst)(i, j);
}

static gsl_vector *seamless_clone_channel(Im *src, Im *dst, Im *mask,
  MaskVariables *mv, size_t c, bool use_mixed)
{
  int w = src->w();
  int h = src->h();
  int n = mask->numWhite();

  gsl_spmatrix *A = gsl_spmatrix_alloc(n, n); // Sparse matrix of constraints
  gsl_spmatrix *C;                            // Compressed column format
  gsl_vector *f = gsl_vector_alloc(n);        // Right-hand side vector
  gsl_vector *u = gsl_vector_alloc(n);        // Solution vector

  /* Create an equation for each pixel in mask (n equations in total) */
  size_t e = 0;                 // Equation index
  Neighbors neighbors;          // Temporary neighbors object
  Coords up, down, left, right; // Temporary coords objects

  for (size_t i = 0; i < w; i++) {
    for (size_t j = 0; j < h; j++) {
      Coords curr(i, j);
      if ((*mask)(curr).isWhite()) {
        neighbors = src->getNeighborCoords(i, j);
        up = neighbors.up;
        down = neighbors.down;
        left = neighbors.left;
        right = neighbors.right;

        /* Pixel coefficient */
        gsl_spmatrix_set(A, e, mv->c2v(curr), neighbors.size());

        /* Neighbor coefficients */
        if (up.valid() && (*mask)(up).isWhite()) {
          gsl_spmatrix_set(A, e, mv->c2v(up), -1.0);
        }
        if (down.valid() && (*mask)(down).isWhite()) {
          gsl_spmatrix_set(A, e, mv->c2v(down), -1.0);
        }
        if (left.valid() && (*mask)(left).isWhite()) {
          gsl_spmatrix_set(A, e, mv->c2v(left), -1.0);
        }
        if (right.valid() && (*mask)(right).isWhite()) {
          gsl_spmatrix_set(A, e, mv->c2v(right), -1.0);
        }

        /* Right-hand side vector value */
        double value = 0.0;

        double scurr  = (*src)(curr)[c];
        double sup    = (*src)(up)[c];
        double sdown  = (*src)(down)[c];
        double sleft  = (*src)(left)[c];
        double sright = (*src)(right)[c];

        double dcurr  = (*dst)(curr)[c];
        double dup    = (*dst)(up)[c];
        double ddown  = (*dst)(down)[c];
        double dleft  = (*dst)(left)[c];
        double dright = (*dst)(right)[c];

        // Boundary conditions
        if (up.valid() && (*mask)(up).isBlack()) {
          value = value + dup;
        }
        if (down.valid() && (*mask)(down).isBlack()) {
          value = value + ddown;
        }
        if (left.valid() && (*mask)(left).isBlack()) {
          value = value + dleft;
        }
        if (right.valid() && (*mask)(right).isBlack()) {
          value = value + dright;
        }

        // Gradient constraints
        if (up.valid()) {
          if (use_mixed) {
            if (abs(scurr - sup) > abs(dcurr - dup)) {
              value = value + (scurr - sup);
            } else {
              value = value + (dcurr - dup);
            }
          } else {
            value = value + (scurr - sup);
          }
        }
        if (down.valid()) {
          if (use_mixed) {
            if (abs(scurr - sdown) > abs(dcurr - ddown)) {
              value = value + (scurr - sdown);
            } else {
              value = value + (dcurr - ddown);
            }
          } else {
            value = value + (scurr - sdown);
          }
        }
        if (left.valid()) {
          if (use_mixed) {
            if (abs(scurr - sleft) > abs(dcurr - dleft)) {
              value = value + (scurr - sleft);
            } else {
              value = value + (dcurr - dleft);
            }
          } else {
            value = value + (scurr - sleft);
          }
        }
        if (right.valid()) {
          if (use_mixed) {
            if (abs(scurr - sright) > abs(dcurr - dright)) {
              value = value + (scurr - sright);
            } else {
              value = value + (dcurr - dright);
            }
          } else {
            value = value + (scurr - sright);
          }
        }

        gsl_vector_set(f, e, value);

        e++; // Increment equation index
      }
    }
  }

  /* Convert to compressed column format */
  C = gsl_spmatrix_ccs(A);

  /* Solve the system with the GMRES iterative solver */
  const double tol = 1.0e-6;     // Tolerance
  const size_t max_iter = 1000;  // Maximum number of iterations
  const gsl_splinalg_itersolve_type *T = gsl_splinalg_itersolve_gmres;
  gsl_splinalg_itersolve *work = gsl_splinalg_itersolve_alloc(T, n, 0);

  size_t iter = 0;
  double residual;
  int status;

  gsl_vector_set_zero(u); // Initial guess; set to zero

  /* Solve the system A u = f */
  do {
    status = gsl_splinalg_itersolve_iterate(C, f, tol, u, work);

    /* Debugging/progress: print out residual norm ||A*u - f|| */
    residual = gsl_splinalg_itersolve_normr(work);
    fprintf(stderr, "iter %zu residual = %.12e\n", iter, residual);

    if (status == GSL_SUCCESS) fprintf(stderr, "Converged\n");
  } while (status == GSL_CONTINUE && ++iter < max_iter);

  gsl_splinalg_itersolve_free(work);

  gsl_spmatrix_free(A);
  gsl_spmatrix_free(C);
  gsl_vector_free(f);

  return u;
}

void seamless_clone(Im *src, Im *dst, Im *mask, Im *out, bool use_mixed)
{
  MaskVariables mv(mask);

  gsl_vector *r_result, *g_result, *b_result;
  r_result = seamless_clone_channel(src, dst, mask, &mv, 0, use_mixed);
  g_result = seamless_clone_channel(src, dst, mask, &mv, 1, use_mixed);
  b_result = seamless_clone_channel(src, dst, mask, &mv, 2, use_mixed);

  /* Create output image */

  // Copy destination image as initial template
  out->copy(dst);

  // Copy over new pixels for masked locations from above solution
  for (size_t i = 0; i < r_result->size; i++) {
    // Read out and bound values
    unsigned char r = (unsigned char) bound(gsl_vector_get(r_result, i));
    unsigned char g = (unsigned char) bound(gsl_vector_get(g_result, i));
    unsigned char b = (unsigned char) bound(gsl_vector_get(b_result, i));

    // Write in new pixel value
    Coords c = mv.v2c(i);
    (*out)(c.x, c.y) = Color(r, g, b);
  }

  /* Clean-up */
  gsl_vector_free(r_result);
  gsl_vector_free(g_result);
  gsl_vector_free(b_result);
}

int main(int argc, char *argv[])
{
  if (argc < 8) {
    fprintf(stderr, "Usage: %s src.png dst.png mask.png out.png \
      <poisson/mixed> <x-offset> <y-offset>\n", argv[0]);
    exit(1);
  }

  const char *srcname  = argv[1];
	const char *dstname  = argv[2];
  const char *maskname = argv[3];
  const char *outname  = argv[4];
  const char *flag     = argv[5];
  int x = atoi(argv[6]);
  int y = atoi(argv[7]);

  bool use_mixed;
  if (strcmp(flag, "mixed") == 0)
    use_mixed = true;
  else if (strcmp(flag, "poisson") == 0)
    use_mixed = false;
  else {
    fprintf(stderr, "argument must be 'mixed' or 'poisson'\n");
    exit(1);
  }

  /* Read in images */
  Im src, dst, mask;
  if (!src.read(srcname) || !dst.read(dstname) || !mask.read(maskname))
    exit(1);

  /* Translate the source image by (x, y) */
  translate(&src, &dst, x, y);

  Im gray_src = src.toGrayscale();
  gray_src.write(outname);

  // /* Perform non-seamless cloning */
  // Im out(mask.w(), mask.h());
  // seamless_clone(&gray_src, &dst, &mask, &out, use_mixed);
  //
  // /* Write image back out */
	// if (!out.write(outname))
	// 	exit(1);

	exit(0);
}
