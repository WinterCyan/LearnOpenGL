//
//  my_ractangle.cpp
//  GLDEMO
//
//  Created by Winter Cyan on 2019/7/3.
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
    
    GLFWwindow *win = glfwCreateWindow(800, 600, "Demo Ractangle", NULL, NULL);
    if (!win){
        std::cout<<"WINDOW CREATE FAILED."<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    
    //    float vertices[] = {
    //        .5f, .5f, 0.0f,
    //        .5f, -.5f, 0.0f,
    //        -.5f, -.5f, 0.0f,
    //        -.5f, .5f, 0.0f
    //    };
    //    unsigned int indices[] = {
    //        0, 1, 3,
    //        1, 2, 3
    //    };
    
    float vertices[] = {
        -.9f, 0.0f, 0.0f,
        -.5f, .9f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.5f, .9f, 0.0f,
        .9f, 0.0f, 0.0f
    };
    float vertices2[] = {
        0.0f, 0.0f, 0.0f,
        0.5f, .9f, 0.0f,
        .9f, 0.0f, 0.0f
    };
    
    glewExperimental = GL_TRUE;
    glewInit();
    
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
    const char* fragment_shader2 =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main(){\n"
    "   FragColor = vec4(1.0f, 0.92f, 0.23f, 1.0f);\n"
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
    
    unsigned int fragShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader2, 1, &fragment_shader2, NULL);
    glCompileShader(fragShader2);
    int frag_succ2;
    char frag_log2[512];
    glGetShaderiv(fragShader2, GL_COMPILE_STATUS, &frag_succ2);
    if(!frag_succ2){
        glGetShaderInfoLog(fragShader2, 512, NULL, frag_log2);
        std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<<frag_log2<<std::endl;
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
    
    unsigned int shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragShader2);
    glLinkProgram(shaderProgram2);
    int prog_succ2;
    char prog_log2[512];
    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &prog_succ2);
    if(!prog_succ2){
        glGetProgramInfoLog(shaderProgram2, 512, NULL, prog_log2);
        std::cout<<"ERROR::PROGRAM::LINK_FAILED\n"<<prog_log2<<std::endl;
    }
    
    //    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
    glDeleteShader(fragShader2);
    
    unsigned int VBO, VAO, VBO2, VAO2;
    //    unsigned int EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices2, GL_STATIC_DRAW);
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
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //        glDrawArrays(GL_TRIANGLES, 2, 3);
        //        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    
    return 0;
}
