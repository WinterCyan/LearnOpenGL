//
// Created by wintercyan on 7/18/20.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "MyShader.h"
#include "Model.h"
#include "toolkit.h"

using namespace std;

void renderBRDFSphere(MyShader pbrShader, glm::vec3 albedo, glm::vec3 F0, float metallic, float roughness, float ao, glm::mat4 model);
void renderPalette(MyShader paletteShader, glm::vec3 color, glm::mat4 model);

int main()
{
//    cout<<"color from #262626: "<<hexColorToFloat(0x262626).r<<" "<<hexColorToFloat(0x262626).g<<" "<<hexColorToFloat(0x262626).b<<endl;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewExperimental = GL_TRUE;
    glewInit();

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // build and compile shaders
    // -------------------------
    MyShader pbrShader(SCRIPT_DIR"2.2.1.pbr.vs.glsl", SCRIPT_DIR"brdf.fs.glsl", NULL);
    MyShader paletteShader(SCRIPT_DIR"brdf.palette.vs.glsl", SCRIPT_DIR"brdf.palette.fs.glsl", NULL);

    // light
    // ------
    glm::vec3 lightPosition = glm::vec3(-10.0f,  10.0f, 10.0f);
    glm::vec3 lightColor = glm::vec3(2800.0f, 2800.0f, 2800.0f);

    // materials properties
    // ------------
    glm::vec3 F0_array[10] = {
            glm::vec3(0.02, 0.02, 0.02),
            glm::vec3(0.03, 0.03, 0.03),
            glm::vec3(0.05, 0.05, 0.05),
            glm::vec3(0.08, 0.08, 0.08),
            glm::vec3(0.17, 0.17, 0.17),
            glm::vec3(0.56, 0.57, 0.58),
            glm::vec3(0.95, 0.64, 0.54),
            glm::vec3(1.00, 0.71, 0.29),
            glm::vec3(0.91, 0.92, 0.92),
            glm::vec3(0.95, 0.93, 0.88)
    };
    int albedo_array[10] = {
            0x262626,
            0x363636,
            0x3d3d3d,
            0x4f4f4f,
            0x737373,
            0xc6c8c8,
            0xf4d4c5,
            0xf9de9a,
            0xf6f6f8,
            0xfaf8f3
    };

    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    pbrShader.use();
    pbrShader.setMat4("projection", projection);
    paletteShader.use();
    paletteShader.setMat4("projection", projection);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.f,0.f,0.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render scene, supplying the convoluted irradiance map to the final shader.
        // ------------------------------------------------------------------------------------------
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        paletteShader.use();
        paletteShader.setMat4("view", view);
        pbrShader.use();
        pbrShader.setMat4("view", view);
        pbrShader.setVec3("camPos", camera.Position);
        glm::vec3 albedo = hexColorToFloat(0xf6f6f8);
        glm::vec3 F0 = glm::vec3(0.91, 0.92, 0.92);
        float metallic = .7;
        float roughness = .7;
        float ao = 1.f;
        for (int i = 0; i < 10; ++i) {
            model = glm::mat4(1.f);
            model = glm::translate(model, glm::vec3(2.5f*(i-5),0.f,0.f));
            renderBRDFSphere(pbrShader, hexColorToFloat(albedo_array[i]), F0_array[i], metallic,roughness,ao,model);
            model = glm::translate(model, glm::vec3(0.f,2.f,0.f));
            model = glm::scale(model, glm::vec3(.7f));
            renderPalette(paletteShader,hexColorToFloat(albedo_array[i]),model);
            model = glm::translate(model, glm::vec3(0.f,-6.f,0.f));
            renderPalette(paletteShader,F0_array[i],model);
        }

        // render light-source as a sphere
        pbrShader.use();
        pbrShader.setVec3("lightPosition", lightPosition);
        pbrShader.setVec3("lightColor", lightColor);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.5f));
        pbrShader.setMat4("model", model);
        renderSphere(1.f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void renderBRDFSphere(MyShader pbrShader, glm::vec3 albedo, glm::vec3 F0, float metallic, float roughness, float ao, glm::mat4 model) {
    pbrShader.use();
    pbrShader.setVec3("albedo", albedo);
    pbrShader.setVec3("f0", F0);
    pbrShader.setFloat("metallic", metallic);
    pbrShader.setFloat("roughness", roughness);
    pbrShader.setFloat("ao",ao);
    pbrShader.setMat4("model", model);
    renderSphere(1.0);
}
void renderPalette(MyShader paletteShader, glm::vec3 color, glm::mat4 model) {
    paletteShader.use();
    paletteShader.setMat4("model", model);
    paletteShader.setVec3("color", color);
    renderQuad(1.f);
}

