#pragma once
#include <glm/glm.hpp>
#include "Shader.h"

class Lampara {
public:
    Lampara(const glm::vec3& pos);

    void render(Shader& shader);

private:
    glm::vec3 posicion;
    glm::vec3 color = glm::vec3(0.18f, 0.18f, 0.20f); // gris oscuro (terror elegante)
};