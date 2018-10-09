#include "imageio++.h"

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
