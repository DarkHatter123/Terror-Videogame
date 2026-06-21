#ifndef MONSTRUO_MANAGER_H
#define MONSTRUO_MANAGER_H

#include "Monstruo.h"
#include <glm/glm.hpp>
<<<<<<< HEAD
#include <string>
=======
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3

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

<<<<<<< HEAD
    void inicializar();
    void resetear();

    void setPosicionAparicion(const glm::vec3& pos) { posicionAparicion = pos; }

=======

    void inicializar();
    void resetear();


    void setPosicionAparicion(const glm::vec3& pos) { posicionAparicion = pos; }

    // Logica principal
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    void activarMonstruo();
    void activarMonstruoEn(const glm::vec3& pos);
    void desactivarMonstruo();
    void actualizar(float deltaTime, const glm::vec3& posJugador, float radioJugador);
    void render(Shader& shader);

<<<<<<< HEAD
=======
    // Verificaciones
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    bool isJugadorAtrapado() const { return jugadorAtrapado; }
    bool isMonstruoActivo() const { return monstruo.estaActivo(); }
    bool isMonstruoPersiguiendo() const { return monstruo.estaPersiguiendo(); }

<<<<<<< HEAD
    const Monstruo& getMonstruo() const { return monstruo; }

    // ==================== ESTO DEBE ESTAR EN PUBLIC ====================
    glm::vec3 getPosicionMonstruo() const { return monstruo.getPosicion(); }
    // ===================================================================

    float getTiempoCongelado() const { return tiempoCongelado; }

    void setMonstruoActivadoPorPuzzle(bool activado) { monstruoActivadoPorPuzzle = activado; }
    bool isMonstruoActivadoPorPuzzle() const { return monstruoActivadoPorPuzzle; }

    void liberarJugador();

    void reset() {
        // Desactivar el monstruo
        monstruo.desactivar();
        // Reiniciar estado
        jugadorAtrapado = false;
        monstruoActivadoPorPuzzle = false;
        tiempoCongelado = 0.0f;
        // La posición del monstruo se reinicia cuando se activa de nuevo
    }

    void setPuedeAtravesarPuertas(bool puede) { monstruo.setPuedeAtravesarPuertas(puede); }
    void setAnimacionPorDefecto(const std::string& anim) { monstruo.setAnimacionPorDefecto(anim); }
    void reproducirAnimacion(const std::string& anim) { monstruo.reproducirAnimacion(anim); }
=======
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
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
};

#endif // MONSTRUO_MANAGER_H