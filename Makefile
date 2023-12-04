TARGET=fillpoly.x

compile: fillpoly.cc
	g++ fillpoly.cc -w -lSDL2 -o $(TARGET)
