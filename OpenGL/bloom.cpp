    //
//  bloom.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/9/23.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "MyShader.h"
#include "MyCamera.h"

#define WIDTH 1200
#define HEIGHT 800
#define TITLE "learn opengl"

using namespace std;

unsigned int loadTexture(char const * path, bool gammaCorrection);
void mouse_callback(GLFWwindow* win, double xpos, double ypos);
void processInput(GLFWwindow* win);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);
void renderCube();
void renderQuad();

float deltaTime = 0.f;
float lastFrame = 0.f;
float lastX = 0.f, lastY = 0.f;
bool firstMouse = true;

bool hdr = true;
bool hdrKeyPressed = false;
float exposure = 1.0f;


MyCamera camera(glm::vec3(0.f, 0.f, 5.f));

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
    
    glEnable(GL_DEPTH_TEST);
    
    // shaders
    MyShader sceneShader("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/vs.bloom.scene", "/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/fs.bloom.scene", NULL);
    sceneShader.use();
    sceneShader.setInt("boxTexture", 0);
    MyShader lightShader("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/vs.bloom.scene", "/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/fs.bloom.light", NULL);
    MyShader quadShader("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/vs.bloom.quad", "/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/fs.bloom.quad", NULL);
    quadShader.use();
    quadShader.setInt("allColor", 0);
    quadShader.setInt("blurredColor", 1);
    MyShader blurShader("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/vs.bloom.quad", "/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/fs.bloom.blur", NULL);
    blurShader.use();
    blurShader.setInt("lightColor", 0);
    
    // textures
    unsigned int boxTexture = loadTexture("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/container2.png", true);
    unsigned int floorTexture = loadTexture("/Users/wintercyan/XCODE/OpenGL/LearnOpenGL/wood.png", true);
    
    // create FBO
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (int i = 0; i < 2; i ++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
    // tell opengl explicitly that we're rendering to multiple color buffers
    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cout<<"incomplete fbo."<<endl;
    }
    
    // create ping-pong FBO
    unsigned int ppFBO[2];
    glGenFramebuffers(2, ppFBO);
    unsigned int ppTextures[2];
    glGenTextures(2, ppTextures);
//    unsigned int ppRBO[2];
//    glGenRenderbuffers(2, ppRBO);
    for (int i = 0; i < 2; i ++ ) {
        glBindTexture(GL_TEXTURE_2D, ppTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindFramebuffer(GL_FRAMEBUFFER, ppFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppTextures[i], 0);
//        glBindRenderbuffer(GL_RENDERBUFFER, ppRBO[i]);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ppRBO[i]);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            cout<<"incomplete pp-fbo."<<endl;
        }
    }
    
    vector<glm::vec3> lightPositions;
    lightPositions.push_back(glm::vec3( 0.0f, 0.5f,  1.5f));
    lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
    lightPositions.push_back(glm::vec3( 3.0f, 0.5f,  1.0f));
    lightPositions.push_back(glm::vec3(-.8f,  2.4f, -1.0f));
    // colors
    vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(5.0f,   5.0f,  5.0f));
    lightColors.push_back(glm::vec3(10.0f,  0.0f,  0.0f));
    lightColors.push_back(glm::vec3(0.0f,   0.0f,  25.0f));
    lightColors.push_back(glm::vec3(0.0f,   5.0f,  0.0f));

    unsigned int iteration = 0;
    while (!glfwWindowShouldClose(window)) {
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        processInput(window);
        
        auto projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, .1f, 100.f);
        auto view = camera.getViewMatrix();
        auto model = glm::mat4(1.f);
        
        glClearColor(.0f, .0f, .0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
//         render to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        sceneShader.use();
        sceneShader.setMat4("projection", projection);
        sceneShader.setMat4("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i ++) {
            sceneShader.setVec3("lights[" + to_string(i) + "].Pos", lightPositions[i]);
            sceneShader.setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
        }
        
        // create one large cube that acts as the floor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
        model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
        sceneShader.setMat4("model", model);
        renderCube();
        
        // then create multiple cubes as the scenery
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, boxTexture);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        model = glm::scale(model, glm::vec3(0.5f));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
        model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(1.25));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
        model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        sceneShader.setMat4("model", model);
        renderCube();
        
        // create lights as bright cubes
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        for (int i = 0; i < lightColors.size(); i ++) {
            model = glm::mat4(1.f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(.25f));
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", lightColors[i]);
            renderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // blur
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        blurShader.use();
        glActiveTexture(GL_TEXTURE0);
        bool first_iteration = true;
        bool horizontal = true;
        for (int i = 0; i < 10; i ++) {
            blurShader.setBool("horizontal", horizontal);
            if (first_iteration) {
                glBindTexture(GL_TEXTURE_2D, colorBuffers[1]); // use bright color at first
                first_iteration = false;
            } else
                glBindTexture(GL_TEXTURE_2D, ppTextures[1]); // use ppTexture-1
            glBindFramebuffer(GL_FRAMEBUFFER, ppFBO[0]); // render to ppFBO-0, ppTexture-0
            renderQuad();
            horizontal = !horizontal;
            
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, ppTextures[0]); // use ppTexture-0
            glBindFramebuffer(GL_FRAMEBUFFER, ppFBO[1]); // render to ppTexture-1
            renderQuad();
            horizontal = !horizontal;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // render quad
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        quadShader.use();
        quadShader.setFloat("exposure", exposure);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ppTextures[1]);
        renderQuad();
        
        iteration ++;
        if (iteration == 20) {
            iteration = 0;
//            cout<<"hdr: "<<(hdr ? "on" : "off") << "|exposure: "<<exposure<<endl;
        }
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

void processInput(GLFWwindow* window){
    float moveSpeed = 3.f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime, moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime, moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime, moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime, moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !hdrKeyPressed)
    {
        hdr = !hdr;
        hdrKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        hdrKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (exposure > 0.0f)
            exposure -= 0.001f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        exposure += 0.001f;
    }
}

void scroll_callback(GLFWwindow* win, double xoffset, double yoffset){
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path, bool gammaCorrection){
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
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

void renderCube(){
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
    if (cubeVAO == 0) {
        float cubeVertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
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
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
}

void renderQuad(){
    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
