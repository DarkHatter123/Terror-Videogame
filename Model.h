#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include <string>
#include <vector>
#include <iostream>

class Model {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    Model(const char* path);
    Model(const Model& other);
    Model& operator=(const Model& other);
    ~Model();

    void Draw(Shader &shader);
    void setManualTexture(const char* texturePath);
    
private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char *path, const std::string &directory);
};