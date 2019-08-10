//
//  advancedLight.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/8/9.
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

unsigned int loadTexture(char const * path);
void mouse_callback(GLFWwindow* win, double xpos, double ypos);
void processInput(GLFWwindow* win);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);

float deltaTime = 0.f;
float lastFrame = 0.f;

MyCamera myCamera(glm::vec3(.0f, .0f, 3.f));
float lastX = 0.f, lastY = 0.f;
bool firstMouse = true;

//glm::vec3 lampPos(1.2f, 1.f, 2.f);

int main(){
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
    
    MyShader floorShader = MyShader("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/vs.adli", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/fs.adli", NULL);
    MyShader lampShader = MyShader("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/v_lamp", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/f_lamp", NULL);
    
    unsigned int floor = loadTexture("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/floor.jpg");
    
    float vertices[] = {
        // pos           // norm        // tex coords
        -5.f, 0.f, -5.f, 0.f, 1.f, 0.f, 0.f, 0.f,
        5.f, 0.f,  -5.f, 0.f, 1.f, 0.f, 1.f, 0.f,
        -5.f, 0.f,  5.f, 0.f, 1.f, 0.f, 0.f, 1.f,
        
        5.f, 0.f, -5.f,  0.f, 1.f, 0.f, 1.f, 0.f,
        -5.f, 0.f, 5.f,  0.f, 1.f, 0.f, 0.f, 1.f,
        5.f, 0.f, 5.f,   0.f, 1.f, 0.f, 1.f, 1.f
    };
    
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glBindVertexArray(floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    while(!glfwWindowShouldClose(win)){
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        
        processInput(win);
        glClearColor(.1f, .1f, .1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        auto projection = glm::perspective(glm::radians(myCamera.Zoom), (float)800/600, .1f, 100.f);
        auto view = myCamera.getViewMatrix();
        auto model = glm::mat4(1.f);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floor);
        
        floorShader.use();
        floorShader.setMat4("model", model);
        floorShader.setMat4("view", view);
        floorShader.setMat4("projection", projection);
        floorShader.setVec3("lightPos", 0, 1, 2);
        floorShader.setVec3("viewPos", myCamera.Position);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    
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

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}
