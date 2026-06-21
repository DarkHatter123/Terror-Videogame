#include "MonstruoManager.h"
#include <iostream>

MonstruoManager::MonstruoManager() {
    inicializar();
}

void MonstruoManager::inicializar() {
    monstruo.inicializar();
    jugadorAtrapado = false;
    tiempoCongelado = 0.0f;
    monstruoActivadoPorPuzzle = false;
    posicionAparicion = glm::vec3(9.5f, -5.0f, 28.0f);
}

void MonstruoManager::resetear() {
    monstruo.resetear();
    jugadorAtrapado = false;
    tiempoCongelado = 0.0f;
    monstruoActivadoPorPuzzle = false;
}

void MonstruoManager::activarMonstruo() {
    activarMonstruoEn(posicionAparicion);
}

void MonstruoManager::activarMonstruoEn(const glm::vec3& pos) {
    glm::vec3 posCorregida = pos;
    posCorregida.y = -5.0f;

    monstruo.activar(posCorregida);
    monstruoActivadoPorPuzzle = true;
    std::cout << "Monstruo activado en posición: ("
              << posCorregida.x << ", " << posCorregida.y << ", " << posCorregida.z << ")" << std::endl;
}

void MonstruoManager::desactivarMonstruo() {
    monstruo.desactivar();
    monstruoActivadoPorPuzzle = false;
}

void MonstruoManager::actualizar(float deltaTime, const glm::vec3& posJugador, float radioJugador) {
    monstruo.actualizar(deltaTime, posJugador);

    // ==================== DEBUG PARA VERIFICAR COLISIÓN ====================
    float dist = glm::distance(monstruo.getPosicion(), posJugador);
    bool colision = monstruo.verificarColision(posJugador, radioJugador);

    static float debugTimer = 0.0f;
    debugTimer += deltaTime;
    if (debugTimer > 0.5f) {
        debugTimer = 0.0f;
        std::cout << "DEBUG: Dist=" << dist << " | Colision=" << (colision ? "SI" : "NO") << " | Atrapado=" << jugadorAtrapado << std::endl;
    }
    // ======================================================================

    if (!jugadorAtrapado && colision) {
        jugadorAtrapado = true;
        tiempoCongelado = 0.0f;
        std::cout << "¡El monstruo te ha atrapado! Cámara congelada." << std::endl;
    }

    if (jugadorAtrapado) {
        tiempoCongelado += deltaTime;
    }
}

void MonstruoManager::render(Shader& shader) {
    monstruo.render(shader);
}

void MonstruoManager::liberarJugador() {
    jugadorAtrapado = false;
    tiempoCongelado = 0.0f;
    std::cout << "Jugador liberado del monstruo." << std::endl;
}