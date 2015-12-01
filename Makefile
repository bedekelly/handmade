COMPILER=g++
FILENAME=main.cpp
BUILD_OUT=../build/main
CFLAGS=`sdl2-config --cflags`
LIBS=`sdl2-config --libs`

main:
	$(COMPILER) $(FILENAME) -o $(BUILD_OUT) $(CFLAGS) $(LIBS)
