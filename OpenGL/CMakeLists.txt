cmake_minimum_required(VERSION 3.16)
project(learnopengl)
link_libraries(GL GLU GLEW glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor assimp)
set(CMAKE_CXX_STANDARD 14)

add_executable(BRDF BRDF.cpp)
add_executable(IBL IBLCopy.cpp)
