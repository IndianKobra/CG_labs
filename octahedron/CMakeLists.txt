cmake_minimum_required(VERSION 3.17)
project(octahedron)

set(CMAKE_CXX_STANDARD 17)

add_executable(octahedron octahedron.cpp)
target_link_libraries(octahedron -lglut -lglfw -lGLEW -lGL -lGLU -lSOIL)