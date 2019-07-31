//
//  model.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/7/21.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include "MyShader.h"
#include <cmath>
#include "MyCamera.h"
#include "Model.h"

using namespace std;

void mouse_callback(GLFWwindow* win, double xpos, double ypos);
void processInput(GLFWwindow* win);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);
float deltaTime = 0.f;
float lastFrame = 0.f;

MyCamera myCamera(glm::vec3(.0f, .0f, .3f));

float lastX = 0.f, lastY = 0.f;
bool firstMouse = true;

int model(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow *win = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
    if (!win){
        std::cout<<"WINDOW CREATE FAILED."<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(win, mouse_callback);
    glfwSetScrollCallback(win, scroll_callback);
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    
    MyShader shader = MyShader("/Users/wintercyan/Documents/XCODE/GL/OpenGL/modelvs", "/Users/wintercyan/Documents/XCODE/GL/OpenGL/modelfs");
    
    
    Model my_model("/Users/wintercyan/Documents/XCODE/GL/OpenGL/nanosuit/nanosuit.obj");
    
    while(!glfwWindowShouldClose(win)){
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        
        processInput(win);
        glClearColor(.05f, .05f, .05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        
        auto projection = glm::perspective(glm::radians(myCamera.Zoom), (float)800/600, .1f, 100.f);
        auto view = myCamera.getViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        auto model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        shader.setMat4("model", model);
        my_model.Draw(shader);
        
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}

void mouse_callback(GLFWwindow* win, double xpos, double ypos){
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    myCamera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* win){
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(win, true);
    
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        myCamera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        myCamera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        myCamera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        myCamera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void scroll_callback(GLFWwindow* win, double xoffset, double yoffset){
    myCamera.ProcessMouseScroll(yoffset);
}
