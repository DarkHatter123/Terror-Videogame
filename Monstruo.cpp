#include "Monstruo.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>

Monstruo::Monstruo() {
    modeloQuieto = nullptr;
    modeloCorre = nullptr;
    inicializar();
    inicializarGraficos();
}

Monstruo::~Monstruo() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    if (modeloQuieto) {
        delete modeloQuieto;
        modeloQuieto = nullptr;
    }
    if (modeloCorre) {
        delete modeloCorre;
        modeloCorre = nullptr;
    }
}

void Monstruo::inicializarGraficos() {
    float vertices[] = {
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };

    unsigned int indices[] = {
        0,  1,  2,  2,  3,  0,
        4,  5,  6,  6,  7,  4,
        8,  9,  10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    indicesCount = sizeof(indices) / sizeof(unsigned int);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Monstruo::inicializar() {
    posicion = glm::vec3(0.0f, -5.0f, 0.0f);
    escala = glm::vec3(1.0f, 1.0f, 1.0f);
    color = glm::vec3(0.8f, 0.1f, 0.1f);
    velocidad = 3.5f;
    activo = false;
    persiguiendo = false;
    puedeAtravesarPuertas = true;
    tiempoDesdeAparicion = 0.0f;
    // ==================== CAMBIO IMPORTANTE ====================
    radioColision = 0.5f;
    // ==========================================================

    rangoVision = 15.0f;
    anguloVision = 120.0f;
    jugadorEnVision = false;
    tiempoPerdidaVision = 0.0f;
    tiempoMaximoPerdida = 3.0f;

    animTime = 0.0f;

    if (!modeloQuieto) {
        modeloQuieto = new Model("models/Monstruo/Monstruo_quieto.glb");
        if (modeloQuieto->getMeshCount() > 0) {
            std::cout << "Modelo quieto cargado" << std::endl;
        }
    }

    if (!modeloCorre) {
        modeloCorre = new Model("models/Monstruo/Monstruo_corre.glb");
        if (modeloCorre->getMeshCount() > 0) {
            std::cout << "Modelo corre cargado" << std::endl;
        }
    }
}

bool Monstruo::puedeVerJugador(const glm::vec3& posJugador) const {
    if (!activo) return false;

    const float LIMITE_X_MIN = 9.5f;
    const float LIMITE_X_MAX = 27.5f;
    const float LIMITE_Z_MIN = 28.0f;
    const float LIMITE_Z_MAX = 46.0f;

    return (posJugador.x >= LIMITE_X_MIN && posJugador.x <= LIMITE_X_MAX &&
            posJugador.z >= LIMITE_Z_MIN && posJugador.z <= LIMITE_Z_MAX);
}

void Monstruo::activar(const glm::vec3& posInicial) {
    posicion = posInicial;
    activo = true;
    persiguiendo = false;
    tiempoDesdeAparicion = 0.0f;
    jugadorEnVision = false;
    tiempoPerdidaVision = 0.0f;
    color = glm::vec3(0.8f, 0.1f, 0.1f);
    if (modeloCorre) modeloCorre->setAnimation(0.0f);
    if (modeloQuieto) modeloQuieto->setAnimation(0.0f);
    std::cout << "¡El monstruo ha aparecido en posición: ("
              << posInicial.x << ", " << posInicial.y << ", " << posInicial.z << ")" << std::endl;
}

void Monstruo::desactivar() {
    activo = false;
    persiguiendo = false;
    jugadorEnVision = false;
    std::cout << "El monstruo ha sido desactivado." << std::endl;
}

void Monstruo::resetear() {
    inicializar();
    std::cout << "El monstruo ha sido reseteado." << std::endl;
}

void Monstruo::actualizar(float deltaTime, const glm::vec3& posJugador) {
    if (!activo) return;

    tiempoDesdeAparicion += deltaTime;

    if (modeloQuieto && modeloQuieto->hasAnimation()) {
        modeloQuieto->updateAnimation(deltaTime);
    }
    if (modeloCorre && modeloCorre->hasAnimation()) {
        modeloCorre->updateAnimation(deltaTime);
    }

    if (!persiguiendo) {
        if (puedeVerJugador(posJugador)) {
            std::cout << "¡DETECTADO! El monstruo te seguirá hasta el fin del mundo." << std::endl;
            persiguiendo = true;
            jugadorEnVision = true;
            color = glm::vec3(1.0f, 0.0f, 0.0f);
            velocidad = 3.5f;
        }
    }

    if (persiguiendo) {
        glm::vec3 direccion = posJugador - posicion;
        float distancia = glm::length(direccion);

        if (distancia > 0.1f) {
            direccion = glm::normalize(direccion);
            direccion.y = 0.0f;
            direccion = glm::normalize(direccion);

            posicion += direccion * velocidad * deltaTime;
            posicion.y = -5.0f;

            float angulo = glm::degrees(atan2(direccion.x, direccion.z));
            if (modeloCorre) modeloCorre->rotation.y = angulo;
        }
    }
}

void Monstruo::render(Shader& shader) {
    if (!activo) return;

    shader.use();
    shader.setVec3("objectColor", color);

    Model* modeloActual = persiguiendo ? modeloCorre : modeloQuieto;

    if (modeloActual && modeloActual->getMeshCount() > 0) {
        modeloActual->position = posicion;
        modeloActual->scale = escala;
        if (!persiguiendo) modeloActual->rotation = glm::vec3(0.0f);
        if (modeloActual->hasAnimation()) {
            modeloActual->DrawAnimated(shader);
        } else {
            modeloActual->Draw(shader);
        }
    } else {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, posicion);
        model = glm::scale(model, escala);
        shader.setMat4("model", model);
        shader.setBool("usarTextura", true);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

bool Monstruo::verificarColision(const glm::vec3& posJugador, float radioJugador) const {
    if (!activo) return false;

    float distancia = glm::distance(posicion, posJugador);

    // ==================== SOLUCIÓN ====================
    // Aumentamos el radio del jugador SOLO para la colisión
    // El radio original es 0.4, lo aumentamos a 1.5
    float radioJugadorEfectivo = 4.0f;  // Ajusta este valor
    // =================================================

    float limite = radioJugadorEfectivo + radioColision;  // 1.5 + 0.5 = 2.0

    return distancia < limite;
}