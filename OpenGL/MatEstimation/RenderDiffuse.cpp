//
// Created by winter on 2021/11/22.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include "../MyShader.h"
#include "../toolkit.h"

using namespace std;
namespace fs = std::filesystem;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    MyShader ndrsEnvShader(SCRIPT_DIR"ndrsEnv.vs.glsl", SCRIPT_DIR"render_diffuse.fs.glsl", NULL);
    MyShader equirectangularToCubemapShader(SCRIPT_DIR"2.2.1.cubemap.vs.glsl", SCRIPT_DIR"2.2.1.equirectangular_to_cubemap.fs.glsl", NULL);
    MyShader irradianceShader(SCRIPT_DIR"2.2.1.cubemap.vs.glsl", SCRIPT_DIR"2.2.1.irradiance_convolution.fs.glsl", NULL);
    MyShader prefilterShader(SCRIPT_DIR"2.2.1.cubemap.vs.glsl", SCRIPT_DIR"2.2.1.prefilter.fs.glsl", NULL);
    MyShader brdfShader(SCRIPT_DIR"2.2.1.brdf.vs.glsl", SCRIPT_DIR"2.2.1.brdf.fs.glsl", NULL);

    ndrsEnvShader.use();
    ndrsEnvShader.setInt("irradianceMap", 0);
    ndrsEnvShader.setInt("prefilterMap", 1);
    ndrsEnvShader.setInt("brdfLUT", 2);
    ndrsEnvShader.setInt("albedoMap", 4);
    ndrsEnvShader.setInt("normalMap", 5);
    ndrsEnvShader.setInt("metallicMap", 6);
    ndrsEnvShader.setInt("roughnessMap", 7);
    ndrsEnvShader.setFloat("ao", 1.0f);
    ndrsEnvShader.setVec3("camPos", camera.Position);

    std::string path = "/home/winter/MEDataset/train";
//    std::string path = "/home/winter/MEDataset/7some";
//    std::string path = "/home/winter/MEDataset/eval";
    int rendered_count = 0;
    clock_t t1 = clock();
//    for (const auto & entry : fs::directory_iterator(path)) {
    for (int name_idx=199069; name_idx>=1; name_idx--) {
        int hdr_idx = rand()%15 + 1;
        string hdr_name = "/home/winter/code/LearnOpenGL/textures/hdr/name.hdr";
        string hdr_path = hdr_name.replace(hdr_name.find("name"), 4, to_string(hdr_idx));
        unsigned int hdrTexture = loadHDRTexture(hdr_path.c_str());
//        unsigned int hdrTexture = loadHDRTexture(TEX_DIR"hdr/18.hdr");

        // WWWWWWWWWWTTTTTTTTTTTTTTFFFFFFFFFFF ?????
        // MUUUUUUUUUUUUUUUUUUUUUUUST load it here ?????????????????
//        string mat_path = entry.path();
        std::string mat_path = "/home/winter/MEDataset/train/"+to_string(name_idx)+".png";
        unsigned int *ndrs = loadNDRS(mat_path.c_str());
        // FUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUCK


        // ----------------------- RBO and FBO --------------------------
        unsigned int captureFBO;
        unsigned int captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBEMAP_SIZE, CUBEMAP_SIZE);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
        // ----------------------- RBO and FBO --------------------------


        // ----------------------- Env CubeMap --------------------------
        unsigned int envCubemap;
        glGenTextures(1, &envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, CUBEMAP_SIZE, CUBEMAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
                {
                        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                                    glm::vec3(0.0f, -1.0f, 0.0f)),
                        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                                    glm::vec3(0.0f, -1.0f, 0.0f)),
                        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f)),
                        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                                    glm::vec3(0.0f, 0.0f, -1.0f)),
                        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                                    glm::vec3(0.0f, -1.0f, 0.0f)),
                        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                                    glm::vec3(0.0f, -1.0f, 0.0f))
                };

        equirectangularToCubemapShader.use();
        equirectangularToCubemapShader.setInt("equirectangularMap", 0);
        equirectangularToCubemapShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        glViewport(0, 0, CUBEMAP_SIZE, CUBEMAP_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i) {
            equirectangularToCubemapShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube(1.f);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        // ----------------------- Env CubeMap --------------------------



        // ----------------------- Irradiance Map --------------------------
        unsigned int irradianceMap;
        glGenTextures(1, &irradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, IRRADIANCEMAP_SIZE, IRRADIANCEMAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, IRRADIANCEMAP_SIZE, IRRADIANCEMAP_SIZE);

        irradianceShader.use();
        irradianceShader.setInt("environmentMap", 0);
        irradianceShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glViewport(0, 0, IRRADIANCEMAP_SIZE, IRRADIANCEMAP_SIZE); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i) {
            irradianceShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube(1.f);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // ----------------------- Irradiance Map --------------------------



        // ----------------------- PreFilter Map --------------------------
        unsigned int prefilterMap;
        glGenTextures(1, &prefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PREFILTEREDMAP_BASE_SIZE, PREFILTEREDMAP_BASE_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        prefilterShader.use();
        prefilterShader.setInt("environmentMap", 0);
        prefilterShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            unsigned int mipWidth = PREFILTEREDMAP_BASE_SIZE * std::pow(0.5, mip);
            unsigned int mipHeight = PREFILTEREDMAP_BASE_SIZE * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float) mip / (float) (maxMipLevels - 1);
            prefilterShader.setFloat("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i) {
                prefilterShader.setMat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderCube(1.f);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // ----------------------- PreFilter Map --------------------------



        // ----------------------- BRDF LUT Map --------------------------
        unsigned int brdfLUTTexture;
        glGenTextures(1, &brdfLUTTexture);

        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, BRDFLUT_SIZE, BRDFLUT_SIZE, 0, GL_RG, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, BRDFLUT_SIZE, BRDFLUT_SIZE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

        glViewport(0, 0, BRDFLUT_SIZE, BRDFLUT_SIZE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        brdfShader.use();
        renderQuad(1.f);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // ----------------------- BRDF LUT Map --------------------------



        // ----------------------- BRDF LUT Map --------------------------
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        glViewport(0, 0, scrWidth, scrHeight);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, ndrs[1]);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ndrs[0]);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, ndrs[3]);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, ndrs[2]);

        glm::mat4 model = glm::mat4(1.0f);
        float R1 = rand()%20 + 80;
        float R2 = rand()%20 - 10;
        model = glm::rotate(model, glm::radians(-R1), glm::vec3(1.f,0.f,0.f));
        model = glm::rotate(model, glm::radians(R2), glm::vec3(0.f,1.f,0.f));
        ndrsEnvShader.use();
        ndrsEnvShader.setMat4("model", model);
        renderQuad(1.0);

        string save_path = mat_path.replace(mat_path.find("train"), 5, "diffuse_train");
//        string save_path = mat_path.replace(mat_path.find("some"), 4, "diffuse_some");
//        string save_path = mat_path.replace(mat_path.find("eval"), 4, "diffuse_eval");
        save_path = save_path.replace(save_path.find(".png"), 4, "___diffuse.png");
        saveImageFromWindow(save_path.c_str(), window);


//        glfwSwapBuffers(window);

        glDeleteBuffers(1, &captureFBO);
        glDeleteBuffers(1, &captureRBO);
        glDeleteTextures(1, &hdrTexture);
        glDeleteTextures(4, ndrs);
        glDeleteTextures(1, &brdfLUTTexture);
        glDeleteTextures(1, &envCubemap);
        glDeleteTextures(1, &irradianceMap);
        glDeleteTextures(1, &prefilterMap);

        rendered_count ++;
        if (rendered_count%10000==0) {
            clock_t t2 = clock();
            float t_cost = ((float)(t2-t1))/CLOCKS_PER_SEC;
            cout<<"10000 imgs cost: "<<t_cost<<" sec."<<endl;
            t1 = clock();
        }
    }

    glfwTerminate();

    return 0;
}
