#include "Lampara.h"
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Lampara::Lampara(const glm::vec3& pos) : posicion(pos) {}

static unsigned int VAO = 0, VBO = 0;
static int vertexCount = 0;

void generarConoTruncado(float radioBase, float radioTop, float altura, int segmentos)
{
    if (VAO != 0) return;

    std::vector<float> vertices;

    for (int i = 0; i < segmentos; i++)
    {
        float ang1 = (2.0f * M_PI * i) / segmentos;
        float ang2 = (2.0f * M_PI * (i + 1)) / segmentos;

        float x1b = cos(ang1) * radioBase;
        float z1b = sin(ang1) * radioBase;
        float x2b = cos(ang2) * radioBase;
        float z2b = sin(ang2) * radioBase;

        float x1t = cos(ang1) * radioTop;
        float z1t = sin(ang1) * radioTop;
        float x2t = cos(ang2) * radioTop;
        float z2t = sin(ang2) * radioTop;

        // Triángulo 1
        vertices.insert(vertices.end(), {
            x1b, 0.0f, z1b,
            x2b, 0.0f, z2b,
            x1t, altura, z1t
        });

        // Triángulo 2
        vertices.insert(vertices.end(), {
            x2b, 0.0f, z2b,
            x2t, altura, z2t,
            x1t, altura, z1t
        });
    }

    vertexCount = vertices.size() / 3;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void renderShape()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void Lampara::render(Shader& shader)
{
    generarConoTruncado(0.9f, 0.35f, 0.7f, 32);

    glm::mat4 model = glm::mat4(1.0f);

    // Pantalla principal
    model = glm::translate(glm::mat4(1.0f), posicion);
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    renderShape();

    // Cuello superior
    model = glm::translate(glm::mat4(1.0f), posicion + glm::vec3(0.0f, 0.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", glm::vec3(0.28f, 0.28f, 0.30f));
    renderShape();

    // Bombilla
    model = glm::translate(glm::mat4(1.0f), posicion + glm::vec3(0.0f, 0.15f, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", glm::vec3(0.9f, 0.75f, 0.45f));
    renderShape();
}