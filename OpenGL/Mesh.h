//
//  Mesh.h
//  OpenGL
//
//  Created by Winter Cyan on 2019/7/23.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//

#ifndef Mesh_h
#define Mesh_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MyShader.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures){
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        
        setupMesh();
    }
    void Draw(MyShader shader){
        // index of two different kind of texture
        unsigned int DIFFUSE_NUM = 1;
        unsigned int SPECULAR_NUM = 1;
        unsigned int AMBIENT_NUM = 1;
        unsigned int NORMALS_NUM = 1;
        unsigned int BASE_COLOR_NUM = 1;
        unsigned int NORMAL_CAMERA_NUM = 1;
        unsigned int METALNESS_NUM = 1;
        unsigned int ROUGHNESS_NUM = 1;
        unsigned int AO_NUM = 1;
        for (int i = 0; i < textures.size(); i ++) {
            glActiveTexture(GL_TEXTURE0 + i);
            string tex_number;  // index number in string
            string tex_type = textures[i].type;
            if (tex_type == "aiTextureType_DIFFUSE")
                tex_number = to_string(DIFFUSE_NUM++);
            else if (tex_type == "aiTextureType_SPECULAR")
                tex_number = to_string(SPECULAR_NUM++);
            else if (tex_type == "aiTextureType_AMBIENT")
                tex_number = to_string(AMBIENT_NUM++);
            else if (tex_type == "aiTextureType_NORMALS")
                tex_number = to_string(NORMALS_NUM++);
            else if (tex_type == "aiTextureType_BASE_COLOR")
                tex_number = to_string(BASE_COLOR_NUM++);
            else if (tex_type == "aiTextureType_NORMAL_CAMERA")
                tex_number = to_string(NORMAL_CAMERA_NUM++);
            else if (tex_type == "aiTextureType_METALNESS")
                tex_number = to_string(METALNESS_NUM++);
            else if (tex_type == "aiTextureType_DIFFUSE_ROUGHNESS")
                tex_number = to_string(ROUGHNESS_NUM++);
            else if (tex_type == "aiTextureType_AMBIENT_OCCLUSION")
                tex_number = to_string(AO_NUM++);

            // set int value for different texture
            //std::cout<<"setup texture name: "<<(tex_type + tex_number).c_str()<<" as "<<i+3<<std::endl;
            glUniform1i(glGetUniformLocation(shader.ID, (tex_type + tex_number).c_str()), i+3); // consider sampler count or not
            //shader.setFloat("material." + tex_type + tex_number, i);    // in one iteration, GL_TEXTURE-i is activated, and meanwhile, e.g., texture_diffuse1 is i, the shader(texture_diffuse1) and program(texture1) is linked with GL_TEXTURE-i
            glActiveTexture(GL_TEXTURE0+i+3);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);   // texture data is stored in texture[i], specified with id, data will be transfered to GL_TEXTURE-i
        }
        
        // draw
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh(){
        // gen buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        // bind buffers & data
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        
        // specify data's structure
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        // tex coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);
        
        glBindVertexArray(0);
    }
};

#endif /* Mesh_h */
