//
//  my_window.cpp
//  GLDEMO
//
//  Created by Winter Cyan on 2019/7/1.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

//void my_resize_callback(GLFWwindow *win, int width, int height){
//    glViewport(0,0,width, height);
//}

int main(){
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
    
    float vertices[] = {
        -.5f, -.5f, 0.0f,
        .5f, -.5f, 0.0f,
        0.0f, .5f, 0.0f
    };
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    const char* vertex_shader =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main(){\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n\0";
    const char* fragment_shader =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main(){\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);
    int vertex_succ;
    char vertex_log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertex_succ);
    if(!vertex_succ){
        glGetShaderInfoLog(vertexShader, 512, NULL, vertex_log);
        std::cout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"<<vertex_log<<std::endl;
    }
    
    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragment_shader, NULL);
    glCompileShader(fragShader);
    int frag_succ;
    char frag_log[512];
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &frag_succ);
    if(!frag_succ){
        glGetShaderInfoLog(fragShader, 512, NULL, frag_log);
        std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<<frag_log<<std::endl;
    }
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    int prog_succ;
    char prog_log[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &prog_succ);
    if(!prog_succ){
        glGetProgramInfoLog(shaderProgram, 512, NULL, prog_log);
        std::cout<<"ERROR::PROGRAM::LINK_FAILED\n"<<prog_log<<std::endl;
    }
    //    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
    
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    while(!glfwWindowShouldClose(win)){
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);
        
        glClearColor(.2f, .3f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    
    return 0;
}
