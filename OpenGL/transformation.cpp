//
//  transformation.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/7/6.
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int transformation(){
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
    glewExperimental = GL_TRUE;
    glewInit();
    
    MyShader shader = MyShader("/Users/wintercyan/Documents/XCODE/GL/OpenGL/vfile.vs", "/Users/wintercyan/Documents/XCODE/GL/OpenGL/ffile.fs");
    
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    unsigned int texture, texture2;
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
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
    unsigned char* pic_data2 = stbi_load("/Users/wintercyan/Documents/XCODE/GL/OpenGL/face.png",&width2, &height2, &nrChannels2, 0);
    if (pic_data2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pic_data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout<<"Failed to load texture data."<<std::endl;
    }
    stbi_image_free(pic_data2);
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    shader.use();
    shader.setInt("myTexture1", 0);
    shader.setInt("myTexture2", 1);

    float c = .5;
    
    while(!glfwWindowShouldClose(win)){
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);
        
        glClearColor(.2f, .3f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        shader.use();
        shader.setFloat("c", c);
        glBindVertexArray(VAO);
        //        glDrawArrays(GL_TRIANGLES, 0, 3);
        //        sleep(1);
        
        glm::mat4 trans = glm::mat4(1.0);
        trans = glm::translate(trans, glm::vec3(.5f, -.5f, .0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        //    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        unsigned int transLocation = glGetUniformLocation(shader.ID, "trans");
        glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        trans = glm::mat4(1.0);
        trans = glm::translate(trans, glm::vec3(-.5f, .5f, .0f));
        trans = glm::scale(trans, glm::vec3(abs(sin(glfwGetTime()))));
        //    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        transLocation = glGetUniformLocation(shader.ID, "trans");
        glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        if (glfwGetKey(win, GLFW_KEY_UP)==GLFW_PRESS) {
            c += .005;
            if (c>=1.0) {
                c = 1.0;
            }
        }
        if (glfwGetKey(win, GLFW_KEY_DOWN)==GLFW_PRESS) {
            c-= .005;
            if (c<=0.0) {
                c=0.0;
            }
        }
        
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    
    return 0;
}
