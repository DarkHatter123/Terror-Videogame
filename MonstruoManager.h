#ifndef MONSTRUO_MANAGER_H
#define MONSTRUO_MANAGER_H

#include "Monstruo.h"
#include <glm/glm.hpp>
#include <string>

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

    void activarMonstruo();
    void activarMonstruoEn(const glm::vec3& pos);
    void desactivarMonstruo();
    void actualizar(float deltaTime, const glm::vec3& posJugador, float radioJugador);
    void render(Shader& shader);

    bool isJugadorAtrapado() const { return jugadorAtrapado; }
    bool isMonstruoActivo() const { return monstruo.estaActivo(); }
    bool isMonstruoPersiguiendo() const { return monstruo.estaPersiguiendo(); }

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
};

#endif // MONSTRUO_MANAGER_H