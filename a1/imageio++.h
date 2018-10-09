#ifndef IMAGEIO_H
#define IMAGEIO_H
/*
Extended by Austin Le

Adapted from Szymon Rusinkiewicz
Princeton University

imageio++.h
Simple wrapper for JPEG and PNG libraries for image input and output.
*/

#include <string>
#include <vector>
#include <stdlib.h>

// Structure defining color at a pixel: R, G, and B as unsigned char,
// with array access also available.
class Color {
public:
  Color() : r(0), g(0), b(0) {}
  Color(unsigned char r_, unsigned char g_, unsigned char b_) :
    r(r_), g(g_), b(b_) {}
	unsigned char r, g, b;
	const unsigned char &operator [] (int i) const { return (&r)[i]; }
	unsigned char &operator [] (int i) { return (&r)[i]; }
  bool isWhite() const { return (r == 255) && (g == 255) && (b == 255); }
  bool isBlack() const { return (r == 0) && (g == 0) && (b == 0); }
};

class Coords {
public:
  Coords() : x(-1), y(-1) {}
  Coords(int x_, int y_) : x(x_), y(y_) {}

  // Boolean check for validity
  bool valid() const { return (x != -1) || (y != -1); }

  // Equality operator
  bool operator==(const Coords &other) const
  { return x == other.x && y == other.y; }

  int x, y;
};

namespace std {
template <>
struct hash<Coords>
{
  std::size_t operator()(const Coords& c) const
  { return ((c.x + c.y) * (c.x + c.y + 1)/2) + c.y; }
};
}

class Neighbors {
public:
  Neighbors() {}
  Neighbors(Coords up_, Coords down_, Coords left_, Coords right_) {
    up = up_;
    down = down_;
    left = left_;
    right = right_;
  }

  size_t size() const {
    size_t s = 0;
    if (up.valid()) s++;
    if (down.valid()) s++;
    if (left.valid()) s++;
    if (right.valid()) s++;
    return s;
  }

  Coords up, down, left, right;
};

// Class defining an image: width, height, and pixels
class Im {
public:
	// Constructors
	Im() : width(0), height(0), size(0) {}
	Im(int width_, int height_) : width(width_), height(height_) {
    size = width_ * height_;
    pixels.resize(width * height);
  }

	// Accessors for private variables
	int w() const { return width; }
	int h() const { return height; }
  int s() const { return size; }

	// Array access.  *No* bounds checking.
	const Color &operator [] (int i) const
		{ return pixels[i]; }
	Color &operator [] (int i)
		{ return pixels[i]; }

	// Access by (x,y) coordinate.  *No* bounds checking.
	const Color &operator () (int x, int y) const
		{ return pixels[x + y * w()]; }
	Color &operator () (int x, int y)
		{ return pixels[x + y * w()]; }

  // Access by Coords object.  *No* bounds checking.
	const Color &operator () (Coords c) const
		{ return pixels[c.x + c.y * w()]; }
	Color &operator () (Coords c)
		{ return pixels[c.x + c.y * w()]; }

  // Return the number of white pixels in this image.
  size_t numWhite() const;

  // Given a (x, y) coordinate, return an object containing references
  // to the valid neighboring coordinates.
  Neighbors getNeighborCoords(int x, int y) const;

  // Copy another image
  void copy(Im *i2);

	// Read an Im from a file.  Returns true if succeeded, else false.
	bool read(const ::std::string &filename);

	// Write an Im to a file.  Returns true if succeeded, else false.
	bool write(const ::std::string &filename);

private:
	int width, height;
  int size;
	::std::vector<Color> pixels;

};
#endif
