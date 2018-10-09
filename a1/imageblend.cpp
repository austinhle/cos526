#include "imageblend.h"

void non_seamless_clone(Im &src, Im &dst, Im &mask, Im &out)
{
  for (int i = 0; i < out.w(); i++)
    for (int j = 0; j < out.h(); j++)
      out(i, j) = mask(i, j).isWhite() ? src(i, j) : dst(i, j);
}

static inline double bound(double d) {
  if (d > 255.0) return 255.0;
  else if (d < 0.0) return 0.0;
  else return d;
}

gsl_vector *seamless_clone_channel(Im *src, Im *dst, Im *mask,
  MaskVariables *mv, size_t c, bool use_mixed)
{
  int w = src->w();
  int h = src->h();
  int n = mask->numWhite();

  gsl_spmatrix *A = gsl_spmatrix_alloc(n, n); // Sparse matrix of constraints
  gsl_spmatrix *C; // Compressed column format
  gsl_vector *f = gsl_vector_alloc(n); // Right-hand side vector
  gsl_vector *u = gsl_vector_alloc(n); // Solution vector

  /* Create an equation for each pixel in mask (n equations in total) */
  size_t e = 0; // Equation index
  Neighbors neighbors; // Temporary neighbors object
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
            if ((scurr - sup) > (dcurr - dup)) {
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
            if ((scurr - sdown) > (dcurr - ddown)) {
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
            if ((scurr - sleft) > (dcurr - dleft)) {
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
            if ((scurr - sright) > (dcurr - dright)) {
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
  const double tol = 1.0e-6;  /* solution relative tolerance */
  const size_t max_iter = 100; /* maximum iterations */
  const gsl_splinalg_itersolve_type *T = gsl_splinalg_itersolve_gmres;
  gsl_splinalg_itersolve *work = gsl_splinalg_itersolve_alloc(T, n, 0);
  size_t iter = 0;
  double residual;
  int status;

  /* initial guess u = 0 */
  gsl_vector_set_zero(u);

  /* solve the system A u = f */
  do {
    status = gsl_splinalg_itersolve_iterate(C, f, tol, u, work);

    /* print out residual norm ||A*u - f|| */
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
  out->copy(dst);

  // Read out values from least-squares solution
  for (size_t i = 0; i < r_result->size; i++) {
    unsigned char r = (unsigned char) bound(gsl_vector_get(r_result, i));
    unsigned char g = (unsigned char) bound(gsl_vector_get(g_result, i));
    unsigned char b = (unsigned char) bound(gsl_vector_get(b_result, i));

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
  if (argc < 6) {
    fprintf(stderr, "Usage: %s src.png dst.png mask.png out.png <bool>\n", argv[0]);
    exit(1);
  }

  const char *srcname  = argv[1];
	const char *dstname  = argv[2];
  const char *maskname = argv[3];
  const char *outname  = argv[4];
  const char *flag     = argv[5];

  bool use_mixed;
  if (strcmp(flag, "true") == 0)
    use_mixed = true;
  else
    use_mixed = false;

  /* Read in images */
  Im src, dst, mask;
  if (!src.read(srcname) || !dst.read(dstname) || !mask.read(maskname))
    exit(1);

  /* Perform non-seamless cloning */
  Im out(src.w(), src.h());
  seamless_clone(&src, &dst, &mask, &out, use_mixed);

  /* Write image back out */
	if (!out.write(outname))
		exit(1);

	exit(0);
}
