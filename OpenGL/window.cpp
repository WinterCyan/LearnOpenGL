//
//  my_window.cpp
//  GLDEMO
//
//  Created by Winter Cyan on 2019/7/1.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

//void my_resize_callback(GLFWwindow *win, int width, int height){
//    glViewport(0,0,width, height);
//}

int window(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow *win = glfwCreateWindow(800, 600, "Demo Window", NULL, NULL);
    if (!win){
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    
    //    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    //        return -1;
    //    }
    
    //    glViewport(0, 0, 800, 600);
    //    glfwSetFramebufferSizeCallback(win, my_resize_callback);
    
    while(!glfwWindowShouldClose(win)){
        glClearColor(.2f, .3f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}
