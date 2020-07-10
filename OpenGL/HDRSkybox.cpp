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
#include "params.h"

#define WIDTH 2560
#define HEIGHT 1440
#define CUBEMAP_SIZE 2560
#define TITLE "learn opengl"

using namespace std;

unsigned int loadTexture(char const * path, bool gammaCorrection);
unsigned int loadHDRTexture(char const * path);
void mouse_callback(GLFWwindow* win, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* win, int button, int action, int mods);
void processInput(GLFWwindow* win);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);
void renderCube(float A);
void renderSphere();
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
    MyShader shader(PROJECT_DIR"vs.pbr",PROJECT_DIR"fs.pbr",NULL);
    MyShader cubeMapShader(PROJECT_DIR"vs.makecubemap", PROJECT_DIR"fs.makecubemap", NULL);
    MyShader skyboxShader(PROJECT_DIR"vs.pbr_skybox", PROJECT_DIR"fs.pbr_skybox", NULL);
    shader.use();
    shader.setVec3("camPos",camera.Position);
    shader.setVec3("albedo",.5f,0.0f,0.0f);
    shader.setFloat("ao", 1.0f);
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
        shader.setVec3("lightPoss["+to_string(i)+"]", lightPoss[i]);
        shader.setVec3("lightColors["+to_string(i)+"]", lightColors[i]);
    }
    cubeMapShader.use();
    cubeMapShader.setInt("equirectangularMap",0);
    skyboxShader.use();
    skyboxShader.setInt("envCubeMap", 0);

    // texture
//    unsigned int albedoTex = loadTexture(PROJECT_DIR"pbrtex/basecolor.png",false);
//    unsigned int metallicTex = loadTexture(PROJECT_DIR"pbrtex/metallic.png",false);
//    unsigned int normalTex = loadTexture(PROJECT_DIR"pbrtex/normal.png",false);
//    unsigned int roughnessTex = loadTexture(PROJECT_DIR"pbrtex/roughness.png",false);
    unsigned int hdrTex = loadHDRTexture(PROJECT_DIR"pbrtex/center.hdr");

    // create frame_buffer & render_buffer
    unsigned int fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBEMAP_SIZE, CUBEMAP_SIZE);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo); // attach render_buffer to frame_buffer
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,rbo); // attach render_buffer to frame_buffer

    // create cubemap
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

    // cubemapProjection
    glm::mat4 cubemapProjection = glm::perspective(glm::radians(90.f),1.f,.1f,10.f);
    glm::mat4 cubemapViews[] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glViewport(0,0,CUBEMAP_SIZE,CUBEMAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    cubeMapShader.use();
    cubeMapShader.setMat4("projection", cubemapProjection);
    for (int i = 0; i < 6; ++i) {
        cubeMapShader.setMat4("view", cubemapViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, envCubeMap, 0); // attach a texture to frame_buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        renderCube(1.f);
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    int screenW, screenH;
    glfwGetFramebufferSize(window, &screenW, &screenH);
    std::cout<<screenW<<" "<<screenH<<std::endl;
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
//        glBindTexture(GL_TEXTURE_2D, hdrTex);
//        renderCube(3.f);

        // render sphere without texture
        shader.use();
        shader.setMat4("projection",projection);
        shader.setMat4("view",view);
        shader.setMat4("model",model);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, albedoTex);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, metallicTex);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, roughnessTex);
//        glActiveTexture(GL_TEXTURE3);
//        glBindTexture(GL_TEXTURE_2D, normalTex);
        for (int row = 0; row < rowSize; row++) {
            shader.setFloat("metallic", (float)row/(float)rowSize);
            for (int col = 0; col < colSize; col++) {
                shader.setFloat("roughness", glm::clamp((float)col/(float)colSize,0.1f,1.0f));
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(((float)col-colSize/2.0)*spacing,((float)row-rowSize/2.0)*spacing,0));
                shader.setMat4("model",model);
                renderSphere();
            }
        }

        // render background
        skyboxShader.use();
        skyboxShader.setMat4("projection",projection);
        skyboxShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        renderCube(1.f);

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

void mouse_button_callback(GLFWwindow* win, int button, int action, int mods) {
    if (glfwGetWindowAttrib(win, GLFW_HOVERED))
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void processInput(GLFWwindow* window){
    float moveSpeed = 3.f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_E)==GLFW_PRESS) {
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    }

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

unsigned int loadHDRTexture(char const * path) {
    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    std::cout<<"loadHDR invoked."<<std::endl;
    float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        std::cout<<"data loaded."<<std::endl;
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout<<"create hdr texture successfully."<<std::endl;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return hdrTexture;
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube(float A){
    if (cubeVAO == 0) {
        float cubeVertices[] = {
                // back face
                -A, -A, -A,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                A,  A, -A,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                A, -A, -A,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                A,  A, -A,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -A, -A, -A,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -A,  A, -A,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -A, -A,  A,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                A, -A,  A,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                A,  A,  A,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                A,  A,  A,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -A,  A,  A,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -A, -A,  A,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -A,  A,  A, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -A,  A, -A, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -A, -A, -A, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -A, -A, -A, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -A, -A,  A, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -A,  A,  A, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                A,  A,  A,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                A, -A, -A,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                A,  A, -A,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
                A, -A, -A,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                A,  A,  A,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                A, -A,  A,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -A, -A, -A,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                A, -A, -A,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                A, -A,  A,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                A, -A,  A,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -A, -A,  A,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -A, -A, -A,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -A,  A, -A,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                A,  A , A,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                A,  A, -A,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
                A,  A,  A,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -A,  A, -A,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -A,  A,  A,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
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

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        const float R = 0.2f;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI)*R;
                float yPos = std::cos(ySegment * PI)*R;
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI)*R;

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}
