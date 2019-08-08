//
//  gs.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/8/7.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//

// basic
#include <iostream>
#include <unistd.h>
#include <cmath>
#include <vector>
// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// headers
#include "Model.h"
#include "MyCamera.h"
#include "MyShader.h"

#define WINDOW_NAME "Learn OpenGL"  // window's name
#define WIDTH 1200
#define HEIGHT 800

using namespace std;

// function declaration
void mouse_callback(GLFWwindow* win, double xpos, double ypos);
void processInput(GLFWwindow* win);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(vector<const char*> faces);
// global variable
float deltaTime = 0.f;
float lastFrame = 0.f;
float lastX = 0.f, lastY = 0.f;
bool firstMouse = true;
MyCamera myCamera(glm::vec3(.0f, .0f, 3.f));

int main() {
    // initiate glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for macos
    // create and setup window
    GLFWwindow *win = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, NULL, NULL);
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
    
    // GL configuration
//    glEnable(GL_DEPTH_TE  ST);
    
    // data
    float vertices[] = {
        -.5f, .5f, 1.f, 0.f, 0.f,
        .5f, .5f, 0.f, 1.f, 0.f,
        .5f, -.5f, 0.f, 0.f, 1.f,
        -.5f, -.5f, 1.f, 1.f, 0.f
    };
    
    // textures
    
    
    // models
    Model my_model("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/nanosuit/nanosuit.obj");
    
    // shaders
    MyShader shader("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/vs.gs", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/fs.gs", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/gs.gs");
    MyShader normShader("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/vs.gs.2", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/fs.gs.2", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/gs.gs.2");
    
    // all buffers and settings
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // render iteration
    while(!glfwWindowShouldClose(win)){
        // calculate frame rate
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        processInput(win);
        
        // clear buffers
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
//         matrix of camera
        auto model = glm::mat4(1.f);
        auto view = myCamera.getViewMatrix();
        auto projection = glm::perspective(glm::radians(myCamera.Zoom), (float)800 / (float)600, .01f, 20.0f);
        
        // bind buffers, configure shaders and draw objects
//        glBindVertexArray(VAO);
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
//        float time = glfwGetTime();
//        shader.setFloat("time", time);
        my_model.Draw(shader);
        
        normShader.use();
        normShader.setMat4("model", model);
        normShader.setMat4("view", view);
        normShader.setMat4("projection", projection);
        my_model.Draw(normShader);
        
//        glDrawArrays(GL_POINTS, 0, 4);
        
        // final operation
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    // storage free up
//    glDeleteVertexArrays(1, &cubeVAO);
//    glDeleteBuffers(1, &cubeVBO);
//    glDeleteBuffers(1, &shaderUBO);
    
    // terminate
    glfwTerminate();
    
    return 0;
}

// other function
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
    
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        myCamera.ProcessKeyboard(FORWARD, deltaTime);
    
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        myCamera.ProcessKeyboard(BACKWARD, deltaTime);
    
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        myCamera.ProcessKeyboard(LEFT, deltaTime);
    
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        myCamera.ProcessKeyboard(RIGHT, deltaTime);
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
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

unsigned int loadCubemap(vector<const char*> faces) {
    unsigned int cubeID;
    glGenTextures(1, &cubeID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeID);
    int width, height, nrChannels;
    for (int i = 0; i < faces.size(); i ++) {
        unsigned char* data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            cout<<"Failed to load cubemap data at: "<<faces[i]<<endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return cubeID;
}
