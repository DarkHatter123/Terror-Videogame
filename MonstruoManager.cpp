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
<<<<<<< HEAD
    posicionAparicion = glm::vec3(9.5f, -5.0f, 28.0f);
=======
    posicionAparicion = glm::vec3(9.5f, -2.5f, 28.0f);
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
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
<<<<<<< HEAD
    glm::vec3 posCorregida = pos;
    posCorregida.y = -5.0f;

    monstruo.activar(posCorregida);
    monstruoActivadoPorPuzzle = true;
    std::cout << "Monstruo activado en posición: ("
              << posCorregida.x << ", " << posCorregida.y << ", " << posCorregida.z << ")" << std::endl;
=======
    monstruo.activar(pos);
    monstruoActivadoPorPuzzle = true;
    std::cout << "Monstruo activado en posición: ("
              << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
}

void MonstruoManager::desactivarMonstruo() {
    monstruo.desactivar();
    monstruoActivadoPorPuzzle = false;
}

void MonstruoManager::actualizar(float deltaTime, const glm::vec3& posJugador, float radioJugador) {
<<<<<<< HEAD
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
=======
    // Actualizar el monstruo
    monstruo.actualizar(deltaTime, posJugador);

    // Verificar si el jugador ha sido atrapado
    //La camara no se congela, ni idea el porque(igual devans lo va a cambiar por la camara de muerte xd)
    if (!jugadorAtrapado && monstruo.verificarColision(posJugador, radioJugador)) {
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
        jugadorAtrapado = true;
        tiempoCongelado = 0.0f;
        std::cout << "¡El monstruo te ha atrapado! Cámara congelada." << std::endl;
    }
<<<<<<< HEAD
=======
    
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3

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