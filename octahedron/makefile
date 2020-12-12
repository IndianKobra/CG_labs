all: run
build: octahedron.cpp SOIL.h
	g++ octahedron.cpp SOIL.h -o check -lglut -lglfw -lGLEW -lGL -lGLU -lSOIL
run: build
	./check