#include "imageio++.h"

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_splinalg.h>

#include <stddef.h>
#include <unordered_map>

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

void translate(Im *src, Im *dst, int x, int y);

void non_seamless_clone(Im &src, Im &dst, Im &mask, Im &out);

static gsl_vector *seamless_clone_channel(Im *src, Im *dst, Im *mask,
  MaskVariables *mv, size_t c, bool use_mixed);

void seamless_clone(Im *src, Im *dst, Im *mask, Im *out, bool use_mixed);
