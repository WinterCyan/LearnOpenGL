//
//  Model.h
//  OpenGL
//
//  Created by Winter Cyan on 2019/7/23.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//

#ifndef Model_h
#define Model_h
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Mesh.h"

using namespace std;

unsigned int TextureFromFile(const char *name, const string &dir, bool gamma = false);

class Model {
public:
    vector<Mesh> meshes;
    string dir;
    vector<Texture> loaded_textures;
    
    Model(string const &path) {
        loadModel(path);
    }
    void Draw(MyShader shader){
        for (int i = 0; i < meshes.size(); i ++) {
            meshes[i].Draw(shader);
        }
    }
private:
    void loadModel(string const &path){
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate|aiProcess_FlipUVs);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout<<"ERROR::ASSIMP::"<<importer.GetErrorString()<<endl;
            return;
        }
        
        dir = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }
    void processNode(aiNode *node, const aiScene *scene){
        // find & save all meshes in this node
        for (unsigned int i = 0; i < node->mNumMeshes; i ++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        
        // recursive to following nodes in this node
        for (unsigned int i = 0; i < node->mNumChildren; i ++) {
            processNode(node->mChildren[i], scene);
        }
    }
    Mesh processMesh(aiMesh *mesh, const aiScene *scene){
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        
        // vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i ++) {
            Vertex vertex;
            
            // position
            glm::vec3 vec;
            vec.x = mesh->mVertices[i].x;
            vec.y = mesh->mVertices[i].y;
            vec.z = mesh->mVertices[i].z;
            vertex.Position = vec;
            
            // normal
            vec.x = mesh->mNormals[i].x;
            vec.y = mesh->mNormals[i].y;
            vec.z = mesh->mNormals[i].z;
            vertex.Normal = vec;
            
            // texture coordinate
            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else {
                vertex.TexCoords = glm::vec2(.0f, .0f);
            }
            
            vertices.push_back(vertex);
        }
        
        // indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i ++) {
            aiFace face = mesh->mFaces[i];  // for every face
            for (unsigned j = 0; j < face.mNumIndices; j ++) {
                indices.push_back(face.mIndices[j]);    // get all indices and push
            }
        }
        
        // material
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex]; // get material for this mesh
            
            // get different textures from material, which have different types
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        
        return Mesh(vertices, indices, textures);
    }
    
    // load textures from a material which contains many different textures
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName){
        vector<Texture> textures;
        
        // for certain type
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str); // get i-th texture of type, and store name in str

            bool skip = false;
            for (int j = 0; j < loaded_textures.size(); j++) {
                if (strcmp(loaded_textures[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(loaded_textures[j]); // if loaded, push the loaded one to textures
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->dir);   // if not loaded, load
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);    // and push it into textures and loaded textures
                loaded_textures.push_back(texture);
            } 
        }
        return textures;
    }
};

unsigned int TextureFromFile(const char *name, const string &dir, bool gamma) {
    string fileName = string(name);
    fileName = dir + '/' + fileName;
    
    unsigned int textureId;
    glGenTextures(1, &textureId);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGBA;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels ==3) format = GL_RGB;
        else format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    } else {
        cout<<"Failed load texture data at "<<name<<endl;
        stbi_image_free(data);
    }
    return textureId;
}
#endif /* Model_h */
