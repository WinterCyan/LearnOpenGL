//
// Created by wintercyan on 7/10/20.
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
#include "toolkit.h"

using namespace std;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glDepthFunc(GL_LEQUAL);

    // shaders
    MyShader pbrShader(PROJECT_DIR"vs.pbr", PROJECT_DIR"fs_copy.pbr", NULL);
    MyShader cubeMapShader(PROJECT_DIR"vs.makecubemap", PROJECT_DIR"fs.makecubemap", NULL);
    MyShader irradianceMapShader(PROJECT_DIR"vs.makecubemap", PROJECT_DIR"fs.makeirradiancemap", NULL);
    MyShader prefilteredMapShader(PROJECT_DIR"vs.makecubemap", PROJECT_DIR"fs_copy.makeprefiltered", NULL);
    MyShader brdfLUTShader(PROJECT_DIR"vs.makebrdfLUT", PROJECT_DIR"fs_copy.makebrdfLUT", NULL);
    MyShader skyboxShader(PROJECT_DIR"vs.pbr_skybox", PROJECT_DIR"fs.pbr_skybox", NULL);
    pbrShader.use();
    pbrShader.setInt("irradianceMap",0);
    pbrShader.setInt("prefilteredMap",1);
    pbrShader.setInt("brdfLUT",2);
    pbrShader.setVec3("camPos", camera.Position);
    pbrShader.setVec3("albedo", .5f, .50f, .50f);
    pbrShader.setFloat("ao", 1.0f);
//    shader.setInt("albedoTex",0);
//    shader.setInt("metallicTex",1);
//    shader.setInt("roughnessTex",2);
//    shader.setInt("normalTex",3);
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
        pbrShader.setVec3("lightPoss[" + to_string(i) + "]", lightPoss[i]);
        pbrShader.setVec3("lightColors[" + to_string(i) + "]", lightColors[i]);
    }
    cubeMapShader.use();
    cubeMapShader.setInt("equirectangularMap",0);
    skyboxShader.use();
    skyboxShader.setInt("envCubeMap", 0);
    prefilteredMapShader.use();
    prefilteredMapShader.setInt("envMap", 0);

    // texture
//    unsigned int albedoTex = loadTexture(PROJECT_DIR"pbrtex/basecolor.png",false);
//    unsigned int metallicTex = loadTexture(PROJECT_DIR"pbrtex/metallic.png",false);
//    unsigned int normalTex = loadTexture(PROJECT_DIR"pbrtex/normal.png",false);
//    unsigned int roughnessTex = loadTexture(PROJECT_DIR"pbrtex/roughness.png",false);
    unsigned int hdrTex = loadHDRTexture(PROJECT_DIR"pbrtex/lobby.hdr");

    // create frame_buffer & render_buffer
    unsigned int fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBEMAP_SIZE, CUBEMAP_SIZE);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo); // attach render_buffer to frame_buffer

    // create hdrcubemap
    unsigned int envCubeMap;
    glGenTextures(1, &envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGB16F,CUBEMAP_SIZE,CUBEMAP_SIZE,0,GL_RGB,GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // render hdrcubemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glViewport(0,0,CUBEMAP_SIZE,CUBEMAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    cubeMapShader.use();
    glm::mat4 makeCubemapProjection = glm::perspective(glm::radians(90.f), 1.f, .1f, 10.f);
    glm::mat4 makeCubemapView[] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    cubeMapShader.setMat4("projection", makeCubemapProjection);
    std::cout<<"rendering cubemap..."<<std::endl;
    for (int i = 0; i < 6; ++i) {
        cubeMapShader.setMat4("view", makeCubemapView[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, envCubeMap, 0); // attach a texture to frame_buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        renderCube(1.f);
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // create irradiance map
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGB16F,IRRADIANCEMAP_SIZE,IRRADIANCEMAP_SIZE,0,GL_RGB,GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // rebind fbo,rbo
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,IRRADIANCEMAP_SIZE,IRRADIANCEMAP_SIZE);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo);

    // render irradiancemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    glViewport(0,0,IRRADIANCEMAP_SIZE,IRRADIANCEMAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    irradianceMapShader.use();
    irradianceMapShader.setInt("envCubeMap",0);
    irradianceMapShader.setMat4("projection", makeCubemapProjection);
    std::cout<<"rendering irradiance map..."<<std::endl;
    for (int i = 0; i < 6; ++i) {
        cubeMapShader.setMat4("view", makeCubemapView[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, irradianceMap, 0); // attach a texture to frame_buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        renderCube(1.f);
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // create prefilteredmap
    unsigned int prefilteredMap;
    glGenTextures(1, &prefilteredMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PREFILTEREDMAP_BASE_SIZE, PREFILTEREDMAP_BASE_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // render prefilteredmap
    prefilteredMapShader.use();
    prefilteredMapShader.setMat4("projection", makeCubemapProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    unsigned int mipLevels = 5;
    std::cout<<"rendering pre-filtered map..."<<std::endl;
    for (unsigned int mip = 0; mip < mipLevels; ++mip) {
        unsigned int width = PREFILTEREDMAP_BASE_SIZE * std::pow(0.5, mip);
        unsigned int height = PREFILTEREDMAP_BASE_SIZE * std::pow(0.5, mip);
        //std::cout<<width<<" * "<<height<<std::endl;
        glBindRenderbuffer(GL_RENDERBUFFER,rbo);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,width,height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo);
        glViewport(0,0,width,height);
        float roughness = float(mip) / float(mipLevels - 1);
        //std::cout<<roughness<<std::endl;
        prefilteredMapShader.setFloat("roughness",roughness);
        std::cout<<"mipmap level "<<mip<<"..."<<std::endl;
        for (int j = 0; j < 6; ++j) {
            prefilteredMapShader.setMat4("view", makeCubemapView[j]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X+j, prefilteredMap, mip); // attach a texture to frame_buffer
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            renderCube(1.f);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // create brdfLUT
    unsigned int brdfLUT;
    glGenTextures(1, &brdfLUT);
    glBindTexture(GL_TEXTURE_2D,brdfLUT);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RG16F,BRDFLUT_SIZE,BRDFLUT_SIZE,0,GL_RG,GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,BRDFLUT_SIZE,BRDFLUT_SIZE);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);
    glViewport(0, 0,BRDFLUT_SIZE,BRDFLUT_SIZE);
    brdfLUTShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    int screenW, screenH;
    glfwGetFramebufferSize(window, &screenW, &screenH);
    glViewport(0,0,screenW,screenH);

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

        // render cubeMap cube
//        cubeMapShader.use();
//        cubeMapShader.setMat4("projection", projection);
//        cubeMapShader.setMat4("view", view);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, prefilteredMap);
//        renderCube(3.f);

        // render sphere without texture
        pbrShader.use();
        pbrShader.setMat4("projection", projection);
        pbrShader.setMat4("view", view);
        pbrShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP,irradianceMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP,prefilteredMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,brdfLUT);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, albedoTex);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, metallicTex);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, roughnessTex);
//        glActiveTexture(GL_TEXTURE3);
//        glBindTexture(GL_TEXTURE_2D, normalTex);
        for (int row = 0; row < rowSize; row++) {
            pbrShader.setFloat("metallic", (float)row / (float)rowSize);
            for (int col = 0; col < colSize; col++) {
                pbrShader.setFloat("roughness", glm::clamp((float)col / (float)colSize, 0.05f, 1.0f));
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(((float)col-colSize/2.0)*spacing,((float)row-rowSize/2.0)*spacing,0));
                pbrShader.setMat4("model", model);
                renderSphere(.2f);
            }
        }

        // render background
        skyboxShader.use();
        skyboxShader.setMat4("projection",projection);
        skyboxShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        renderCube(1.f);

        // render brdfLUT
//        glViewport(0, 0,BRDFLUT_SIZE,BRDFLUT_SIZE);
//        brdfLUTShader.use();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
