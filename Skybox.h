#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Shader.h"

class Skybox {
private:
    unsigned int VAO, VBO, EBO;
    Shader* shader;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int indexCount;
    
    void generarEsfera(float radio, int sectorCount, int stackCount);

public:
    Skybox(float radio = 50.0f);
    ~Skybox();
    void render(const glm::mat4& view, const glm::mat4& projection);
};