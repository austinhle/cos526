# Author: Austin Le
CXX = g++
CPPFLAGS = -O -std=c++11
LDLIBS = -lm

all: icp
clean:
	rm -f icp *.o

icp: icp.o pointcloud.o matrix4x4.o matrix6x6.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
icp.o: pointcloud.h vector3D.h matrix4x4.h vector4D.h matrix6x6.h vector6D.h
pointcloud.o: pointcloud.h matrix4x4.h vector4D.h
matrix4x4.o: matrix4x4.h
matrix6x6.o: matrix6x6.h
