TARGET=fillpoly.x

compile: src/fillpoly.cc
	g++ src/fillpoly.cc -w -lSDL2 -lSDL2_image -lSDL2_ttf -o $(TARGET)
