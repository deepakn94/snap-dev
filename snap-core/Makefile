#
# Small makefile for testing with non-Microsoft compilers
#

include ../Makefile.config

all: testSnap testSnapMultimodal benchmarkSnapMultimodal benchmarkSnapMultimodalMAG benchmarkSnapMultimodalSynthetic

testSnap: testSnap.cpp Snap.o 
	$(CC) $(CXXFLAGS) -o testSnap testSnap.cpp Snap.o -I$(CGLIB) $(LDFLAGS) $(LIBS)

testSnapMultimodal: testSnapMultimodalGraphs.cpp Snap.o
	$(CC) $(CXXFLAGS) -o testSnapMultimodal testSnapMultimodalGraphs.cpp Snap.o -I$(CGLIB) $(LDFLAGS) $(LIBS)

benchmarkSnapMultimodal: benchmarkMultimodalGraphs.cpp Snap.o
	$(CC) $(CXXFLAGS) -o benchmarkSnapMultimodal benchmarkMultimodalGraphs.cpp Snap.o -I$(CGLIB) $(LDFLAGS) $(LIBS)

benchmarkSnapMultimodalMAG: benchmarkMultimodalGraphsMAG.cpp Snap.o
	$(CC) $(CXXFLAGS) -o benchmarkSnapMultimodalMAG benchmarkMultimodalGraphsMAG.cpp Snap.o -I$(CGLIB) $(LDFLAGS) $(LIBS)

benchmarkSnapMultimodalSynthetic: benchmarkMultimodalGraphsSynthetic.cpp Snap.o
	$(CC) $(CXXFLAGS) -o benchmarkSnapMultimodalSynthetic benchmarkMultimodalGraphsSynthetic.cpp Snap.o -I$(CGLIB) $(LDFLAGS) $(LIBS)

Snap.o: *.h *.cpp ../glib-core/*.h ../glib-core/*.cpp
	$(CC) -c $(CXXFLAGS) Snap.cpp -I$(CGLIB)

lib: Snap.o
	rm -f libsnap.a
	ar -cvq libsnap.a Snap.o

libinstall: lib
	cp libsnap.a /usr/local/lib/libsnap.a

clean:
	rm -f *.o *.a testSnap testSnap.exe *.plt *.tab *.png

