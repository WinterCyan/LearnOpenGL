//
// Created by wintercyan on 7/6/20.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "MyShader.h"
#include "MyCamera.h"
#include "Model.h"
//#include "params.h"
#include "toolkit.h"

using namespace std;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
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
    MyShader shader(SCRIPT_DIR"vs.pbr",SCRIPT_DIR"fs_texture.pbr",NULL);
    MyShader cubeMapShader(SCRIPT_DIR"vs.makecubemap", SCRIPT_DIR"fs.makecubemap", NULL);
    MyShader skyboxShader(SCRIPT_DIR"vs.pbr_skybox", SCRIPT_DIR"fs.pbr_skybox", NULL);
//    MyShader shader(PROJECT_DIR"vs.makecubemap", PROJECT_DIR"fs.makecubemap", NULL);
//    MyShader shader(PROJECT_DIR"vs.pbr",PROJECT_DIR"fs_copy.pbr",NULL);
    shader.use();
    shader.setVec3("camPos",camera.Position);
//    shader.setVec3("albedo",.5f,0.0f,0.0f);
    shader.setFloat("ao", 1.0f);
    shader.setInt("albedoTex",0);
    shader.setInt("metallicTex",1);
    shader.setInt("roughnessTex",2);
    shader.setInt("normalTex",3);
//    shader.setInt("aoTex",4);

    const float LIGHT_DISTANCE = 20.f;
    const float LIGHT_BRIGHTNESS = 1700.f;
    glm::vec3 lightPoss[] = {
            glm::vec3(-LIGHT_DISTANCE,  LIGHT_DISTANCE, LIGHT_DISTANCE),
            glm::vec3( LIGHT_DISTANCE,  LIGHT_DISTANCE, LIGHT_DISTANCE),
            glm::vec3(-LIGHT_DISTANCE, -LIGHT_DISTANCE, LIGHT_DISTANCE),
            glm::vec3( LIGHT_DISTANCE, -LIGHT_DISTANCE, LIGHT_DISTANCE),
    };
    glm::vec3 lightColors[] = {
            glm::vec3(LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS),
            glm::vec3(LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS),
            glm::vec3(LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS),
            glm::vec3(LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS, LIGHT_BRIGHTNESS)
    };
    int rowSize = 7;
    int colSize = 7;
    float spacing = .5f;

    for (int i = 0; i < sizeof(lightPoss); ++i) {
        shader.setVec3("lightPoss["+to_string(i)+"]", lightPoss[i]);
        shader.setVec3("lightColors["+to_string(i)+"]", lightColors[i]);
    }

    cubeMapShader.use();
    cubeMapShader.setInt("equirectangularMap",0);

    // textures
    unsigned int albedoTex = loadTexture(TEX_DIR"iron/basecolor.png");
    unsigned int metallicTex = loadTexture(TEX_DIR"iron/metallic.png");
    unsigned int normalTex = loadTexture(TEX_DIR"iron/normal.png");
    unsigned int roughnessTex = loadTexture(TEX_DIR"iron/roughness.png");

    unsigned int hdrTex = loadHDRTexture(TEX_DIR"hdr/lobby.hdr");

    // create frame_buffer & render_buffer
    unsigned int fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo); // attach render_buffer to frame_buffer
    // create cubemap
    unsigned int cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGB16F,512,512,0,GL_RGB,GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);


    vector<const char*> faces = {
            TEX_DIR"skybox/right.jpg",
            TEX_DIR"skybox/left.jpg",
            TEX_DIR"skybox/top.jpg",
            TEX_DIR"skybox/bottom.jpg",
            TEX_DIR"skybox/back.jpg",
            TEX_DIR"skybox/front.jpg"
    };
    unsigned int cubeMap = loadCubemap(faces);


    // cubemapProjection
    auto cubemapProjection = glm::perspective(glm::radians(90.f),1.f,.1f,10.f);
    glm::mat4 cubemapViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    cubeMapShader.use();
    cubeMapShader.setMat4("projection",cubemapProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glViewport(0,0,512,512);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (int i = 0; i < 6; ++i) {
        cubeMapShader.setMat4("view", cubemapViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,cubemap,0); // attach a texture to frame_buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        renderCube(1.);
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    int screenW, screenH;
    glfwGetFramebufferSize(window, &screenW, &screenH);
    std::cout<<screenW<<" "<<screenH<<std::endl;
    glViewport(0,0,screenW,screenH);

    skyboxShader.use();
    skyboxShader.setInt("cubemap", 0);


    auto projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, .1f, 100.f);
    shader.use();
    shader.setMat4("projection", projection);
    skyboxShader.use();
    skyboxShader.setMat4("projection", projection);

    while (!glfwWindowShouldClose(window)) {
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        processInput(window);

        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, .1f, 100.f);
        auto view = camera.getViewMatrix();
        auto model = glm::mat4(1.f);

//        cubeMapShader.use();
//        cubeMapShader.setMat4("projection", projection);
//        cubeMapShader.setMat4("view", view);
//        model = glm::translate(model, glm::vec3(3.0f,0.0,0.0));
//        cubeMapShader.setMat4("model", model);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, hdrTex);
//        renderCube(1.);

        shader.use();
//        shader.setMat4("projection",projection);
        shader.setMat4("view",view);
        shader.setMat4("model",model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metallicTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, roughnessTex);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, normalTex);
        for (int row = 0; row < rowSize; row++) {
            shader.setFloat("metallic", (float)row/(float)rowSize);
            for (int col = 0; col < colSize; col++) {
                shader.setFloat("roughness", glm::clamp((float)col/(float)colSize,0.1f,1.0f));
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(((float)col-colSize/2.0)*spacing,((float)row-rowSize/2.0)*spacing,0));
                shader.setMat4("model",model);
                renderSphere(0.2);
            }
        }

        skyboxShader.use();
//        skyboxShader.setMat4("projection",projection);
        skyboxShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        renderCube(1.);

//        for (unsigned int i = 0; i < sizeof(lightPoss) / sizeof(lightPoss[0]); ++i)
//        {
//            glm::vec3 newPos = lightPoss[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
//            newPos = lightPoss[i];
//            shader.setVec3("lightPoss[" + std::to_string(i) + "]", lightPoss[i]);
//            shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

//            model = glm::mat4(1.0f);
//            model = glm::translate(model, newPos);
//            model = glm::scale(model, glm::vec3(0.5f));
//            shader.setMat4("model", model);
//            renderSphere();
//        eMapShader.setInt("equirectangularMap",0);}

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}