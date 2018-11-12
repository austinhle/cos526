# Author: Austin Le
CXX = g++
CPPFLAGS = -O
LDLIBS = -lm

all: meshreg
clean:
	rm -f meshreg *.o

meshreg: meshreg.o pointcloud.o matrix4x4.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
meshreg.o: pointcloud.h matrix4x4.h vector4D.h
pointcloud.o: pointcloud.h matrix4x4.h vector4D.h
matrix4x4.o: matrix4x4.h
