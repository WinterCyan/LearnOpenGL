//
//  shadow.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/8/10.
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

#define WINDOW_NAME "Learn OpenGL"  // window's name
#define WIDTH 1200
#define HEIGHT 800

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
        glEnable(GL_DEPTH_TEST);
    
    MyShader floorShader("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/vs.adli", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/fs.adli", NULL);
    MyShader shadowDepthShader("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/vs.shadow", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/fs.shadow", NULL);
    MyShader shadowVisualShader("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/vs.shadowVisual", "/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/fs.shadowVisual", NULL);
    
    unsigned int floorTex = loadTexture("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/floor.jpg");
    unsigned int cubeTex = loadTexture("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/container.jpg");
    
    float floorVer[] = {
        // pos           // norm        // tex coords
        -5.f, 0.f, -5.f, 0.f, 1.f, 0.f, 0.f, 0.f,
        5.f, 0.f,  -5.f, 0.f, 1.f, 0.f, 1.f, 0.f,
        -5.f, 0.f,  5.f, 0.f, 1.f, 0.f, 0.f, 1.f,
        
        5.f, 0.f, -5.f,  0.f, 1.f, 0.f, 1.f, 0.f,
        -5.f, 0.f, 5.f,  0.f, 1.f, 0.f, 0.f, 1.f,
        5.f, 0.f, 5.f,   0.f, 1.f, 0.f, 1.f, 1.f
    };
    float cubeVertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };
    
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glBindVertexArray(floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVer), &floorVer, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // framebuffer
    unsigned int depthFrameBuffer;
    glGenFramebuffers(1, & depthFrameBuffer);
    // depth TEXTURE of framebuffer
    unsigned int depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // attach depth texture to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glDrawBuffer(GL_NONE);  // in case of incomplete
    glReadBuffer(GL_NONE);  // in case of incomplete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) {
        cout<<"ERROR::FRAMEBUFFER::framebuffer not complete."<<endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // shadow map
    auto cubePos = glm::vec3(.5f, 1.f, .5f);
    auto lightPos = glm::vec3(0.f, 5.f, 0.f);
    auto lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);
    auto lightView = glm::lookAt(lightPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    auto lightTransform = lightProjection * lightView;
    
    auto projection = glm::perspective(glm::radians(myCamera.Zoom), (float)WIDTH/HEIGHT, .1f, 100.f);
    auto view = myCamera.getViewMatrix();
    auto model = glm::mat4(1.f);
    
    while(!glfwWindowShouldClose(win)){
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        
        processInput(win);
        glClearColor(.1f, .1f, .1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        // draw depth texture
        shadowDepthShader.use();
        model = glm::translate(model, cubePos);
        shadowDepthShader.setMat4("model", model);
        shadowDepthShader.setMat4("lightTransform", lightTransform);
        glViewport(0, 0, WIDTH, HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
        glClear(GL_DEPTH_BUFFER);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTex);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTex);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // draw visualize
        shadowVisualShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
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

unsigned int loadTexture(char const * path){
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
