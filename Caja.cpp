#include "Caja.h"

Caja::Caja() {
    modelo = nullptr;
    posicion = glm::vec3(0.0f);
    escala = glm::vec3(1.0f);
    rotacionY = 0.0f;
    tieneColision = true;
}

Caja::Caja(Model* model, glm::vec3 pos, glm::vec3 esc, float rot) {
    modelo = model;
    posicion = pos;
    escala = esc;
    rotacionY = rot;
    tieneColision = true;
}

void Caja::render(Shader& shader) {
    if (!modelo) return;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, posicion);
    model = glm::rotate(model, glm::radians(rotacionY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, escala);

    shader.setMat4("model", model);
    shader.setBool("usarTextura", true);

    modelo->Draw(shader);  // ← Pasar el objeto Shader, NO shader.ID
}

bool Caja::verificarColision(glm::vec3 jugadorPos, float radioJugador) const {
    if (!tieneColision) return false;

    glm::vec3 minCaja = posicion - escala / 2.0f - glm::vec3(radioJugador);
    glm::vec3 maxCaja = posicion + escala / 2.0f + glm::vec3(radioJugador);

    return (jugadorPos.x >= minCaja.x && jugadorPos.x <= maxCaja.x &&
            jugadorPos.y >= minCaja.y && jugadorPos.y <= maxCaja.y &&
            jugadorPos.z >= minCaja.z && jugadorPos.z <= maxCaja.z);
}