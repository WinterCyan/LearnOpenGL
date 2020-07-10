cmake_minimum_required(VERSION 3.16)
project(learnopengl)

set(CMAKE_CXX_STANDARD 14)

add_executable(learnopengl HDRSkybox.cpp params.h)

target_link_libraries(learnopengl GL GLU GLEW glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor assimp)