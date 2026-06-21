#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "Shader.h"

class Caja {
private:
    Model* modelo;
    glm::vec3 posicion;
    glm::vec3 escala;
    float rotacionY;
    bool tieneColision;

public:
    // Constructores
    Caja();
    Caja(Model* model, glm::vec3 pos, glm::vec3 esc, float rot = 0.0f);

    // Setters
    void setModelo(Model* model) { modelo = model; }
    void setPosicion(glm::vec3 pos) { posicion = pos; }
    void setEscala(glm::vec3 esc) { escala = esc; }
    void setRotacion(float rot) { rotacionY = rot; }

    // Getters
    glm::vec3 getPosicion() const { return posicion; }
    glm::vec3 getEscala() const { return escala; }
    bool tieneColisionActivada() const { return tieneColision; }

    // Renderizado
    void render(Shader& shader);

    // Colisión
    bool verificarColision(glm::vec3 jugadorPos, float radioJugador) const;
};