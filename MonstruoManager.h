#ifndef MONSTRUO_MANAGER_H
#define MONSTRUO_MANAGER_H

#include "Monstruo.h"
#include <glm/glm.hpp>

class MonstruoManager {
private:
    Monstruo monstruo;
    bool jugadorAtrapado;
    float tiempoCongelado;
    bool monstruoActivadoPorPuzzle;
    glm::vec3 posicionAparicion;

public:
    MonstruoManager();
    ~MonstruoManager() = default;


    void inicializar();
    void resetear();


    void setPosicionAparicion(const glm::vec3& pos) { posicionAparicion = pos; }

    // Logica principal
    void activarMonstruo();
    void activarMonstruoEn(const glm::vec3& pos);
    void desactivarMonstruo();
    void actualizar(float deltaTime, const glm::vec3& posJugador, float radioJugador);
    void render(Shader& shader);

    // Verificaciones
    bool isJugadorAtrapado() const { return jugadorAtrapado; }
    bool isMonstruoActivo() const { return monstruo.estaActivo(); }
    bool isMonstruoPersiguiendo() const { return monstruo.estaPersiguiendo(); }

    // Getters
    const Monstruo& getMonstruo() const { return monstruo; }
    glm::vec3 getPosicionMonstruo() const { return monstruo.getPosicion(); }
    float getTiempoCongelado() const { return tiempoCongelado; }

    // Para el sistema de puzzle junto con el monstruo
    void setMonstruoActivadoPorPuzzle(bool activado) { monstruoActivadoPorPuzzle = activado; }
    bool isMonstruoActivadoPorPuzzle() const { return monstruoActivadoPorPuzzle; }

    // Para liberar al jugador
    void liberarJugador();

    // Permite que el monstruo atraviese puertas
    void setPuedeAtravesarPuertas(bool puede) { monstruo.setPuedeAtravesarPuertas(puede); }
};

#endif // MONSTRUO_MANAGER_H