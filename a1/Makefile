# Extended by Austin Le
# Adapted from Szymon Rusinkiewicz

CPPFLAGS = -O
LDLIBS = -lm -ljpeg -lpng -lgsl -lgslcblas

all: imageio++_test imageblend
clean:
	rm -f imageio++_test imageblend *.o

imageio++_test: imageio++_test.o imageio++.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
imageio++_test.o: imageio++.h
imageio++.o: imageio++.h

imageblend: imageblend.o imageio++.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
imageblend.o: imageio++.h
