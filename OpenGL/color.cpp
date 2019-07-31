//
//  color.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/7/10.
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

void mouse_callback(GLFWwindow* win, double xpos, double ypos);
void processInput(GLFWwindow* win);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);

float deltaTime = 0.f;
float lastFrame = 0.f;

MyCamera myCamera(glm::vec3(.0f, .0f, 3.f));
float lastX = 0.f, lastY = 0.f;
bool firstMouse = true;

//glm::vec3 lampPos(1.2f, 1.f, 2.f);

int color(){
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
    
    MyShader objectShader = MyShader("/Users/wintercyan/Documents/XCODE/GL/OpenGL/v_object", "/Users/wintercyan/Documents/XCODE/GL/OpenGL/f_object");
    MyShader lampShader = MyShader("/Users/wintercyan/Documents/XCODE/GL/OpenGL/v_lamp", "/Users/wintercyan/Documents/XCODE/GL/OpenGL/f_lamp");
    
    float vertices[] = {
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
    
    glm::vec3 cubePos[] = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    
    unsigned int objectVAO, lightVAO, VBO;
    glGenVertexArrays(1, &lightVAO);
    glGenVertexArrays(1, &objectVAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(objectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    int width, height, channel;
    unsigned char* data = stbi_load("/Users/wintercyan/Documents/XCODE/GL/OpenGL/container2.png", &width, &height, &channel, 0);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout<<"Failed load texture data."<<std::endl;
    }
    stbi_image_free(data);
    
    unsigned char* data_spec = stbi_load("/Users/wintercyan/Documents/XCODE/GL/OpenGL/lighting_maps_specular_color.png", &width, &height, &channel, 0);
    unsigned int texture_spec;
    glGenTextures(1, &texture_spec);
    glBindTexture(GL_TEXTURE_2D, texture_spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data_spec) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_spec);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout<<"Failed load texture data."<<std::endl;
    }
    stbi_image_free(data_spec);
    
    unsigned char* data_emit = stbi_load("/Users/wintercyan/Documents/XCODE/GL/OpenGL/emit.jpg", &width, &height, &channel, 0);
    unsigned int texture_emit;
    glGenTextures(1, &texture_emit);
    glBindTexture(GL_TEXTURE_2D, texture_emit);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data_emit) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_emit);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout<<"Failed load texture data."<<std::endl;
    }
    stbi_image_free(data_emit);
    
    while(!glfwWindowShouldClose(win)){
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        
        processInput(win);
        glClearColor(.1f, .1f, .1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_spec);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_emit);
        
        objectShader.use();
        objectShader.setVec3("objectColor", 1.f, .5f, .31f);
        objectShader.setVec3("lightColor", 1.f, 1.f, 1.f);
        
        objectShader.setVec3("material.ambient", 0.f, .05f, .05f);
        objectShader.setVec3("material.diffuse", .4f, .5f, .5f);
        objectShader.setVec3("material.specular", .04f, .7f, .7f);
        objectShader.setFloat("material.shininess", .078125f * 128);
        
        objectShader.setFloat("lamp.cutOff", glm::cos(glm::radians(12.5f)));
        objectShader.setFloat("lamp.outCutOff", glm::cos(glm::radians(17.5f)));
        
        for (int i = 0; i < 4; i ++) {
            objectShader.setVec3("pointLamp[i].ambient", .2f, .2f, .2f);
            objectShader.setVec3("pointLamp[i].diffuse",  1.f, 1.f, 1.f);
            objectShader.setVec3("pointLamp[i].specular", 1.f, 1.f, 1.f);
            objectShader.setFloat("pointLamp[i].constant", 1.f);
            objectShader.setFloat("pointLamp[i].linear", .007f);
            objectShader.setFloat("pointLamp[i].quadratic", .0002f);
            objectShader.setVec3("pointLamp[i].position", pointLightPositions[i]);
        }
        
        objectShader.setVec3("direLamp.direction", 0.f, -1.f, -2.f);
        objectShader.setVec3("direLamp.ambient", .2f, .2f, .2f);
        objectShader.setVec3("direLamp.diffuse",  1.f, 1.f, 1.f);
        objectShader.setVec3("direLamp.specular", 1.f, 1.f, 1.f);
        
        objectShader.setVec3("spotLamp.position", myCamera.Position);
        objectShader.setVec3("spotLamp.direction", myCamera.Front);
        objectShader.setFloat("spotLamp.cutOff", glm::cos(glm::radians(15.f)));
        objectShader.setFloat("spotLamp.outCutOff", glm::cos(glm::radians(17.f)));
        objectShader.setVec3("spotLamp.ambient", .2f, .2f, .2f);
        objectShader.setVec3("spotLamp.diffuse",  1.f, 1.f, 1.f);
        objectShader.setVec3("spotLamp.specular", 1.f, 1.f, 1.f);
        
        objectShader.setVec3("viewPos", myCamera.Position);
        
        objectShader.setInt("material.diffuse", 0);
        objectShader.setInt("material.specular", 1);
        objectShader.setInt("material.emit", 2);
        auto projection = glm::perspective(glm::radians(myCamera.Zoom), (float)800/600, .1f, 100.f);
        auto view = myCamera.getViewMatrix();
        auto model = glm::mat4(1.f);
        for (int i = 0; i < 10; i++) {
            auto model_cubes = glm::mat4(1.f);
            //        model = glm::scale(model, glm::vec3(.5f, 1.f, 1.5f));
            //        model = glm::translate(model, glm::vec3(.5f, .5f, 0.f));
            model_cubes = glm::translate(model_cubes, cubePos[i]);
            model_cubes = glm::rotate(model_cubes, glm::radians((float)20*i), glm::vec3(.0f, .5f, 1.f));
            objectShader.setMat4("projection", projection);
            objectShader.setMat4("view", view);
            objectShader.setMat4("model", model_cubes);
            glBindVertexArray(objectVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        glBindVertexArray(lightVAO);
        for (int j = 0; j < 4; j ++) {
            model = glm::mat4(1.f);
            model = glm::translate(model, pointLightPositions[j]);
            model = glm::scale(model, glm::vec3(.2f));
            lampShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteBuffers(1, &VBO);
    
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
