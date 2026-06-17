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
    posicionAparicion = glm::vec3(9.5f, -2.5f, 28.0f);
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
    monstruo.activar(pos);
    monstruoActivadoPorPuzzle = true;
    std::cout << "Monstruo activado en posición: ("
              << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
}

void MonstruoManager::desactivarMonstruo() {
    monstruo.desactivar();
    monstruoActivadoPorPuzzle = false;
}

void MonstruoManager::actualizar(float deltaTime, const glm::vec3& posJugador, float radioJugador) {
    // Actualizar el monstruo
    monstruo.actualizar(deltaTime, posJugador);

    // Verificar si el jugador ha sido atrapado
    //La camara no se congela, ni idea el porque(igual devans lo va a cambiar por la camara de muerte xd)
    if (!jugadorAtrapado && monstruo.verificarColision(posJugador, radioJugador)) {
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