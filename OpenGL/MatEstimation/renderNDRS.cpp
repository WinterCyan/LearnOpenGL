//
// Created by winter on 2021/11/13.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "../MyShader.h"
#include "../MyCamera.h"
#include "../Model.h"
//#include "params.h"
#include "../toolkit.h"

using namespace std;

#define NDRS_WIDTH 588
#define NDRS_HEIGHT 588

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(NDRS_WIDTH, NDRS_HEIGHT, TITLE, NULL, NULL);
    if (!window) {
        cout<<"create window failed."<<endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // shaders
    MyShader shader(SCRIPT_DIR"renderNDRS.vs.glsl",SCRIPT_DIR"renderNDRS.fs.glsl",NULL);

    shader.use();
    shader.setVec3("camPos",camera.Position);
    shader.setInt("albedoMap",0);
    shader.setInt("normalMap",1);
    shader.setInt("metallicMap",2);
    shader.setInt("roughnessMap",3);
    shader.setInt("aoMap",4);

//    const float LIGHT_DISTANCE = 1.f;
//    const float LIGHT_BRIGHTNESS = 2700.f;
//    glm::vec3 lightPoss[] = {
//            glm::vec3(-LIGHT_DISTANCE,  LIGHT_DISTANCE, LIGHT_DISTANCE),
//            glm::vec3( LIGHT_DISTANCE,  LIGHT_DISTANCE, LIGHT_DISTANCE),
//            glm::vec3(-LIGHT_DISTANCE, -LIGHT_DISTANCE, LIGHT_DISTANCE),
//            glm::vec3( LIGHT_DISTANCE, -LIGHT_DISTANCE, LIGHT_DISTANCE),
//    };
//    glm::vec3 lightColors[] = {
//            glm::vec3(LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS),
//            glm::vec3(LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS),
//            glm::vec3(LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS),
//            glm::vec3(LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS)
//    };
//    int rowSize = 7;
//    int colSize = 7;
//    float spacing = .5f;

//    for (int i = 0; i < sizeof(lightPoss); ++i) {
//        shader.setVec3("lightPoss["+to_string(i)+"]", lightPoss[i]);
//        shader.setVec3("lightColors["+to_string(i)+"]", lightColors[i]);
//    }

    auto LIGHT_COLOR = 30;
    auto lightPos = glm::vec3(0.5f, 0.5f, 1.f);
    auto lightColor = glm::vec3(LIGHT_COLOR, LIGHT_COLOR, LIGHT_COLOR);
    shader.setVec3("lightPosition", lightPos);
    shader.setVec3("lightColor", lightColor);

    // textures
//    unsigned int albedoMap = loadTexture(TEX_DIR"iron/basecolor.png");
//    unsigned int normalMap = loadTexture(TEX_DIR"iron/normal.png");
//    unsigned int metallicMap = loadTexture(TEX_DIR"iron/metallic.png");
//    unsigned int roughnessMap = loadTexture(TEX_DIR"iron/roughness.png");
    unsigned int aoMap = loadTexture(TEX_DIR"iron/ao.png");
    unsigned int* ndrs = loadNDRS(TEX_DIR"ndrs/77.png");

    // create frame_buffer & render_buffer
//    unsigned int fbo, rbo;
//    glGenFramebuffers(1, &fbo);
//    glGenRenderbuffers(1, &rbo);
//    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo); // attach render_buffer to frame_buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    int screenW, screenH;
    glfwGetFramebufferSize(window, &screenW, &screenH);
    glViewport(0,0,screenW,screenH);

    auto projection = glm::perspective(glm::radians(camera.Zoom), (float)NDRS_WIDTH/(float)NDRS_HEIGHT, .1f, 100.f);
    shader.use();
    shader.setMat4("projection", projection);

    while (!glfwWindowShouldClose(window)) {
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        processInput(window);

        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto view = camera.getViewMatrix();
        auto model = glm::mat4(1.f);
//        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, .0f, .0f));

        shader.use();
        shader.setMat4("view",view);
        shader.setMat4("model",model);

        // DNSR 1032
        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, albedoMap);
        glBindTexture(GL_TEXTURE_2D, ndrs[1]);

        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, normalMap);
        glBindTexture(GL_TEXTURE_2D, ndrs[0]);

        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, metallicMap);
        glBindTexture(GL_TEXTURE_2D, ndrs[3]);

        glActiveTexture(GL_TEXTURE3);
//        glBindTexture(GL_TEXTURE_2D, roughnessMap);
        glBindTexture(GL_TEXTURE_2D, ndrs[2]);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, aoMap);

        renderQuad(1.0);

//        model = glm::translate(model, lightPos);
//        shader.setMat4("model", model);
//        renderCube(0.1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
