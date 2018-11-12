# Author: Austin Le
CXX = g++
CPPFLAGS = -O
LDLIBS = -lm

all: icp
clean:
	rm -f icp *.o

icp: icp.o pointcloud.o matrix4x4.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
icp.o: pointcloud.h matrix4x4.h vector4D.h
pointcloud.o: pointcloud.h matrix4x4.h vector4D.h
matrix4x4.o: matrix4x4.h
