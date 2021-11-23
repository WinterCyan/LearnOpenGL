#!/bin/bash
period=2000
for ((i = 1 ; i <= 199069 ; i+=2000))
do
    /home/winter/code/LearnOpenGL/OpenGL/cmake-build-debug/RenderDiffuse $i $period
done
