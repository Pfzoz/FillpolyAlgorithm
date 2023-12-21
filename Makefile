TARGET=fillpoly.x
TEMP_TARGET=temp_fillpoly

compile: src/fillpoly.cc
	g++ src/fillpoly.cc -w -lSDL2 -lSDL2_image -lSDL2_ttf -o $(TARGET)

run: src/fillpoly.cc
	g++ src/fillpoly.cc -w -lSDL2 -lSDL2_image -lSDL2_ttf -o $(TEMP_TARGET) && ./$(TEMP_TARGET) && rm ./$(TEMP_TARGET)