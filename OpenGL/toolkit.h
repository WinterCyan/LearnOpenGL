//
// Created by wintercyan on 7/12/20.
//

#ifndef LEARNOPENGL_TOOLKIT_H
#define LEARNOPENGL_TOOLKIT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "MyShader.h"
#include "MyCamera.h"
#include "Model.h"

#define WIDTH 3840
#define HEIGHT 2160
#define CUBEMAP_SIZE 2560
#define IRRADIANCEMAP_SIZE 128
#define PREFILTEREDMAP_BASE_SIZE 1024
#define BRDFLUT_SIZE 1024
#define TITLE "learn opengl"
#define PROJECT_DIR "/home/wintercyan/code/clion/LearnOpenGL/"

unsigned int loadTexture(char const * path, bool gammaCorrection);
unsigned int loadHDRTexture(char const * path);
void mouse_callback(GLFWwindow* win, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* win, int button, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* win);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);
void renderCube(float A);
void renderSphere(float R);
void renderQuad(float A);
glm::vec3 hexColorToFloat(int hexValue);

float deltaTime = 0.f;
float lastFrame = 0.f;
float lastX = 0.f, lastY = 0.f;
bool firstMouse = true;

bool hdr = true;
bool hdrKeyPressed = false;
float exposure = 1.0f;

MyCamera camera(glm::vec3(0.f, 0.f, 5.f));

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

unsigned int loadTexture(char const * path, bool gammaCorrection) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    return textureID;
}


unsigned int loadHDRTexture(char const * path) {
    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
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

void renderQuad(float A){
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -A,  A, 0.0f, 0.0f, 1.0f,
                -A, -A, 0.0f, 0.0f, 0.0f,
                A,  A, 0.0f, 1.0f, 1.0f,
                A, -A, 0.0f, 1.0f, 0.0f,
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
void renderSphere(float R)
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

        const unsigned int X_SEGMENTS = 512;
        const unsigned int Y_SEGMENTS = 512;
        const float PI = 3.14159265359;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

glm::vec3 hexColorToFloat(int hexValue)
{
    glm::vec3 rgbColor;
    rgbColor.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
    rgbColor.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
    rgbColor.b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte

    return rgbColor;
}

#endif //LEARNOPENGL_TOOLKIT_H
