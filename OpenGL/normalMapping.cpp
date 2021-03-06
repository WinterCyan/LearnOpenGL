//
//  normalMapping.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/9/16.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "MyShader.h"
#include "MyCamera.h"

#define WIDTH 1200
#define HEIGHT 800
#define TITLE "learn opengl"

using namespace std;

unsigned int loadTexture(char const * path);
void mouse_callback(GLFWwindow* win, double xpos, double ypos);
void processInput(GLFWwindow* win);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);
void renderQuad();

float deltaTime = 0.f;
float lastFrame = 0.f;
float lastX = 0.f, lastY = 0.f;
bool firstMouse = true;


MyCamera camera(glm::vec3(0.f, 0.f, 1.f));

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if (!window) {
        cout<<"create window failed."<<endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    MyShader shader("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/vs.parallaxmapping", "/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/fs.parallaxmapping", NULL);
    shader.use();
    shader.setInt("wallTexture", 0);
    shader.setInt("normalTexture", 1);
    shader.setInt("heightTexture", 2);
    
    
    unsigned int wallTexture = loadTexture("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/wood.png");
    unsigned int normalTexture = loadTexture("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/toy_box_normal.png");
    unsigned int heightTexture =
        loadTexture("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/toy_box_disp.png");
    
//    unsigned int wallTexture = loadTexture("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/bricks2.jpg");
//    unsigned int normalTexture = loadTexture("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/bricks2_normal.jpg");
//    unsigned int heightTexture =
//    loadTexture("/Users/wintercyan/Documents/XCODE/OpenGL/LearnOpenGL/bricks2_disp.jpg");
    
    auto lightPos = glm::vec3(.5f, 1.f, .3f);
    while (!glfwWindowShouldClose(window)) {
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        processInput(window);
        
        auto projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, .1f, 50.f);
        auto view = camera.getViewMatrix();
        auto model = glm::mat4(1.f);
//        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
//        model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        
        glClearColor(.0f, .0f, .0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setFloat("height_scale", .1f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, heightTexture);
        renderQuad();
        
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
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
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* win){
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(win, true);
    
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void scroll_callback(GLFWwindow* win, double xoffset, double yoffset){
    camera.ProcessMouseScroll(yoffset);
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

void renderQuad() {
    unsigned int VAO = 0;
    unsigned int VBO;
    
    if (VAO == 0) {
        // position
        glm::vec3 pos1(-1.f, 1.f, 0.f);
        glm::vec3 pos2(-1.f, -1.f, 0.f);
        glm::vec3 pos3(1.f, -1.f, 0.f);
        glm::vec3 pos4(1.f, 1.f, 0.f);
        // texture coordinate
        glm::vec2 uv1(0.f, 1.f);
        glm::vec2 uv2(0.f, 0.f);
        glm::vec2 uv3(1.f, 0.f);
        glm::vec2 uv4(1.f, 1.f);
        // normal
        glm::vec3 nm(0.f, 0.f, 1.f);
        
        // two edges of one triangle
        glm::vec3 m = pos2 - pos1;
        glm::vec3 n = pos3 - pos1;
        glm::vec2 delta_uv1 = uv2 - uv1;
        glm::vec2 delta_uv2 = uv3 - uv1;
        float alpha = delta_uv1.x;
        float beta = delta_uv1.y;
        float gamma = delta_uv2.x;
        float tau = delta_uv2.y;
        glm::vec3 T1, B1;
        float denominator = alpha*tau - gamma*beta;
        T1 = glm::normalize(glm::vec3(tau*m.x - beta*n.x, tau*m.y - beta*n.y, tau*m.z - beta*n.z) / denominator);
        B1 = glm::normalize(glm::vec3(-gamma*m.x + alpha*n.x, -gamma*m.y + alpha*n.y, -gamma*m.z + alpha*n.z) / denominator);
        
        // two edges of another triangle
//        glm::vec3 m = pos3 - pos1;
//        glm::vec3 n = pos4 - pos1;
//        glm::vec2 delta_uv1 = uv3 - uv1;
//        glm::vec2 delta_uv2 = uv4 - uv1;
//        float alpha = delta_uv1.x;
//        float beta = delta_uv1.y;
//        float gamma = delta_uv2.x;
//        float tau = delta_uv2.y;
//        glm::vec3 T2, B2;
//        float denominator = alpha*tau - gamma*beta;
        m = pos3 - pos1;
        n = pos4 - pos1;
        delta_uv1 = uv3 - uv1;
        delta_uv2 = uv4 - uv1;
        alpha = delta_uv1.x;
        beta = delta_uv1.y;
        gamma = delta_uv2.x;
        tau = delta_uv2.y;
        glm::vec3 T2, B2;
        denominator = alpha*tau - gamma*beta;
        T2 = glm::normalize(glm::vec3(tau*m.x - beta*n.x, tau*m.y - beta*n.y, tau*m.z - beta*n.z) / denominator);
        B2 = glm::normalize(glm::vec3(-gamma*m.x + alpha*n.x, -gamma*m.y + alpha*n.y, -gamma*m.z + alpha*n.z) / denominator);
        
        float quadVertices[] = {
            // position             // normal         // tex-coord   // tan            // bi-tan
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, T1.x, T1.y, T1.z, B1.x, B1.y, B1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, T1.x, T1.y, T1.z, B1.x, B1.y, B1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, T1.x, T1.y, T1.z, B1.x, B1.y, B1.z,
            
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, T2.x, T2.y, T2.z, B2.x, B2.y, B2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, T2.x, T2.y, T2.z, B2.x, B2.y, B2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, T2.x, T2.y, T2.z, B2.x, B2.y, B2.z,
        };
        
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)(8*sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)(11*sizeof(float)));
        glEnableVertexAttribArray(4);
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
