#include "imageio++.h"

void non_seamless_cloning(Im &src, Im &dst, Im &mask, Im &out)
{
  for (int i = 0; i < out.w(); i++)
    for (int j = 0; j < out.h(); j++)
      out(i, j) = mask(i, j).is_white() ? src(i, j) : dst(i, j);
}

void seamless_cloning(Im &src, Im &dst, Im &mask, Im &out)
{
  
}

int main(int argc, char *argv[])
{
  if (argc < 5) {
		fprintf(stderr, "Usage: %s src.png dst.png mask.png out.png\n", argv[0]);
		exit(1);
	}

  const char *srcname  = argv[1];
	const char *dstname  = argv[2];
  const char *maskname = argv[3];
  const char *outname  = argv[4];

  /* Read in images */
  Im src, dst, mask;
  if (!src.read(srcname) || !dst.read(dstname) || !mask.read(maskname))
    exit(1);

  /* Perform non-seamless cloning */
  Im out(src.w(), src.h());
  non_seamless_cloning(src, dst, mask, out);

  /* Write image back out */
	if (!out.write(outname))
		exit(1);

	exit(0);
}
