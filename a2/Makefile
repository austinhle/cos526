# Author: Austin Le
CXX = g++
CPPFLAGS = -O -std=c++11
LDLIBS = -lm -lgsl -lgslcblas

all: icp
clean:
	rm -f icp *.o

icp: icp.o point.o pointcloud.o matrix4x4.o matrix6x6.o vector3D.o vector4D.o vector6D.o kdtree.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
icp.o: point.h pointcloud.h vector3D.h matrix4x4.h vector4D.h matrix6x6.h vector6D.h
point.o: point.h matrix4x4.h vector4D.h
pointcloud.o: pointcloud.h matrix4x4.h vector4D.h vector3D.h kdtree.h
matrix4x4.o: matrix4x4.h vector4D.h
matrix6x6.o: matrix6x6.h vector6D.h
vector3D.o: vector3D.h
vector4D.o: vector4D.h
vector6D.o: vector6D.h
kdtree.o: kdtree.h vector3D.h
