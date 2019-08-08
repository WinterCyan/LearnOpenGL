//
//  MyShader.h
//  OpenGL
//
//  Created by Winter Cyan on 2019/7/4.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//

#ifndef MyShader_h
#define MyShader_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

class MyShader{
public:
    unsigned int ID;
//    MyShader(const GLchar* vertexPath, GLchar* fragmentPath);
//    void use();
//    void setBool(const std::string& name, bool value) const;
//    void setInt(const std::string& name, int value) const;
//    void setFloat(const std::string& name, float value) const;

    MyShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath){
        std::string vShaderCode, fShaderCode;
        std::fstream vShaderFile, fShaderFile;
        
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vShaderCode = vShaderStream.str();
            fShaderCode = fShaderStream.str();
        } catch (std::ifstream::failure e) {
            std::cout<<"ERROR::SHADER::FILE_NOT_PROPERLY_LOAD"<<std::endl;
        }
        
        const char* vCode = vShaderCode.c_str();
        const char* fCode = fShaderCode.c_str();
        
        unsigned int vShader, fShader;
        int vSucc, fSucc, pSucc;
        char vLog[512], fLog[512], pLog[512];
        
        vShader = glCreateShader(GL_VERTEX_SHADER);
        fShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vShader, 1, &vCode, NULL);
        glShaderSource(fShader, 1, &fCode, NULL);
        
        glCompileShader(vShader);
        glGetShaderiv(vShader, GL_COMPILE_STATUS, &vSucc);
        if (!vSucc) {
            glGetShaderInfoLog(vShader, 512, NULL, vLog);
            std::cout<<"vShader compile err.\n"<<vLog<<std::endl;
        }
        
        glCompileShader(fShader);
        glGetShaderiv(fShader, GL_COMPILE_STATUS, &fSucc);
        if (!fSucc) {
            glGetShaderInfoLog(fShader, 512, NULL, fLog);
            std::cout<<"fShader compile err.\n"<<fLog<<std::endl;
        }
        
        unsigned int gShader = 0;
        if (geometryPath) {
            std::string gShaderCode;
            std::fstream gShaderFile;
            
            gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            
            try {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                gShaderCode = gShaderStream.str();
            } catch (std::ifstream::failure e) {
                std::cout<<"ERROR::SHADER::FILE_NOT_PROPERLY_LOAD"<<std::endl;
            }
            
            const char* gCode = gShaderCode.c_str();
            
            int gSucc;
            char gLog[512];
            
            gShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(gShader, 1, &gCode, NULL);
            
            glCompileShader(gShader);
            glGetShaderiv(gShader, GL_COMPILE_STATUS, &gSucc);
            if (!gSucc) {
                glGetShaderInfoLog(gShader, 512, NULL, gLog);
                std::cout<<"gShader compile err.\n"<<gLog<<std::endl;
            }
        }
        
        ID = glCreateProgram();
        glAttachShader(ID, vShader);
        glAttachShader(ID, fShader);
        if (geometryPath)
            glAttachShader(ID, gShader);
        glLinkProgram(ID);
        
        glGetProgramiv(ID, GL_LINK_STATUS, &pSucc);
        if (!pSucc) {
            glGetProgramInfoLog(ID, 512, NULL, pLog);
            std::cout<<"program link err.\n"<<pLog<<std::endl;
        }
        
        glDeleteShader(vShader);
        glDeleteShader(fShader);
    }

    void use(){
        glUseProgram(ID);
    }

    void setBool(const std::string& name, bool value) const{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string& name, int value) const{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat4(const std::string& name, float value[4]) const{
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2], value[3]);
    }
    void setMat4(const std::string& name, glm::mat4 &value) const{
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec3(const std::string& name, glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    
};
#endif /* MyShader_h */
