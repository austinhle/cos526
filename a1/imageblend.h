#include "imageio++.h"

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_splinalg.h>

#include <stddef.h>
#include <unordered_map>

/* This class is a wrapper around two unordered_maps that provide bidirectional
   mapping between an (x, y) coordinate within a mask to its index in
   linear system of equations in the Discrete Poisson solver. */
class MaskVariables {
public:
  MaskVariables(Im *mask_) : mask(mask_) {
    int k = 0;
    for (size_t i = 0; i < mask->w(); i++)
      for (size_t j = 0; j < mask->h(); j++)
        if ((*mask)(i, j).isWhite()) {
          coords2index[Coords(i, j)] = k;
          index2coords[k] = Coords(i, j);
          k++;
        }
  }

  // Given a Coords object, return its variable index in the system of equations.
  int c2v(Coords c) { return coords2index[c]; }

  // Given a variable index, return the corresponding Coords object.
  Coords v2c(int v) { return index2coords[v]; }

private:
  Im *mask;
  std::unordered_map<Coords, int> coords2index;
  std::unordered_map<int, Coords> index2coords;
};

/* Place src image into new image with dst's dimensions and then translate
   it by (x, y). */
void translate(Im *src, Im *dst, int x, int y);

/* Perform a non-seamless clone without any blending at all. (i.e. copy
   and paste) */
void non_seamless_clone(Im &src, Im &dst, Im &mask, Im &out);

/* Perform image blending on a single color channel specified by c. mv
   specifies a mapping of pixels in the mask to their corresponding equation
   index. */
static gsl_vector *seamless_clone_channel(Im *src, Im *dst, Im *mask,
  MaskVariables *mv, size_t c, bool use_mixed);

/* Perform image blending of the src image into the dst image using
   the provided mask. use_mixed is true if mixed gradients
   method should be used instead. */
void seamless_clone(Im *src, Im *dst, Im *mask, Im *out, bool use_mixed);
