CC = g++ -std=gnu++11
CFLAGS = -pg -g -O2 -Wall
LDFLAGS = -pg -g
LIBS = -lm

raytracer:
	g++ -std=gnu++11  -o raytracer main.cpp raytracer.cpp util.cpp light_source.cpp scene_object.cpp bmp_io.cpp $(LIBS)
clean:
	-rm -f core *.o
	-rm raytracer