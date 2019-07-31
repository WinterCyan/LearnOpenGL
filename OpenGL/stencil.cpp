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

MyCamera myCamera(glm::vec3(.0f, .0f, .3f));

float lastX = 0.f, lastY = 0.f;
bool firstMouse = true;

int stencil(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow *win = glfwCreateWindow(800, 600, "Demo Triangle", NULL, NULL);
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
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    MyShader shader = MyShader("/Users/wintercyan/Documents/XCODE/GL/OpenGL/vs.depth", "/Users/wintercyan/Documents/XCODE/GL/OpenGL/fs.depth");
    MyShader shader_outline = MyShader("/Users/wintercyan/Documents/XCODE/GL/OpenGL/vs.depth", "/Users/wintercyan/Documents/XCODE/GL/OpenGL/fs.depth.2");
    
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* pic_data = stbi_load("/Users/wintercyan/Documents/XCODE/GL/OpenGL/container.jpg",&width, &height, &nrChannels, 0);
    if (pic_data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pic_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout<<"Failed to load texture data."<<std::endl;
    }
    stbi_image_free(pic_data);
    
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width2, height2, nrChannels2;
    unsigned char* pic_data2 = stbi_load("/Users/wintercyan/Documents/XCODE/GL/OpenGL/emit.jpg",&width2, &height2, &nrChannels2, 0);
    if (pic_data2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, pic_data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout<<"Failed to load texture data."<<std::endl;
    }
    stbi_image_free(pic_data2);
    
    unsigned int VBO1, VBO2, VAO1, VAO2;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    shader.use();
    shader.setInt("texture1", 0);
    
    while(!glfwWindowShouldClose(win)){
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);
        
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        processInput(win);
        
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        auto model = glm::mat4(1.f);
        auto view = myCamera.getViewMatrix();
        auto projection = glm::perspective(glm::radians(myCamera.Zoom), (float)800 / (float)600, .01f, 20.0f);
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader_outline.use();
        shader_outline.setMat4("view", view);
        shader_outline.setMat4("projection", projection);
        
        shader.use();
        // floor
        glStencilMask(0x00);    // disable write to buffer
        glBindVertexArray(VAO2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // draw normal object
        glStencilFunc(GL_ALWAYS, 1, 0xff);  // always pass, set shape-values to 1
        glStencilMask(0xff);    // enable write buffer
        glBindVertexArray(VAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(2.f, 0.f, 0.f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //draw bigger object
        glStencilFunc(GL_NOTEQUAL, 1, 0xff);    // if not equal(out of shape), draw
        glStencilMask(0x00);    // disable write buffer
        glDisable(GL_DEPTH_TEST);   // before draw outline, disable depth test
        shader_outline.use();
        glBindVertexArray(VAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        float scale = 1.05f;
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shader_outline.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(2.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shader_outline.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glStencilMask(0xff);
        glEnable(GL_DEPTH_TEST);
        
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &VBO1);
    
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
