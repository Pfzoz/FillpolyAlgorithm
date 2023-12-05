TARGET=fillpoly.x

compile: fillpoly.cc
	g++ fillpoly.cc -w -lSDL2 -lSDL2_image -lSDL2_ttf -o $(TARGET)
