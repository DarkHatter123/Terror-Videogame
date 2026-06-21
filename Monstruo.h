#ifndef MONSTRUO_H
#define MONSTRUO_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Model.h"
#include <string>

class Monstruo {
private:
    glm::vec3 posicion;
    glm::vec3 escala;
    glm::vec3 color;
    float velocidad;
    bool activo;
    bool persiguiendo;
    bool puedeAtravesarPuertas;
    float tiempoDesdeAparicion;
    float radioColision;
    
    float rangoVision;
    float anguloVision;
    bool jugadorEnVision;
    float tiempoPerdidaVision;
    float tiempoMaximoPerdida;

    Model* modeloQuieto;
    Model* modeloCorre;

    float animTime;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int indicesCount;

    void inicializarGraficos();
    bool puedeVerJugador(const glm::vec3& posJugador) const;

public:
    Monstruo();
    ~Monstruo();

    void inicializar();
    void activar(const glm::vec3& posInicial);
    void desactivar();
    void resetear();

    void actualizar(float deltaTime, const glm::vec3& posJugador);
    void render(Shader& shader);
    void setAnimacionPorDefecto(const std::string& anim) {}
    void reproducirAnimacion(const std::string& anim) {}

    bool verificarColision(const glm::vec3& posJugador, float radioJugador) const;

    glm::vec3 getPosicion() const { return posicion; }
    void setPosicion(const glm::vec3& pos) { posicion = pos; }
    float getVelocidad() const { return velocidad; }
    void setVelocidad(float vel) { velocidad = vel; }
    bool getPuedeAtravesarPuertas() const { return puedeAtravesarPuertas; }
    void setPuedeAtravesarPuertas(bool puede) { puedeAtravesarPuertas = puede; }
    glm::vec3 getColor() const { return color; }
    void setColor(const glm::vec3& col) { color = col; }
    bool estaActivo() const { return activo; }
    bool estaPersiguiendo() const { return persiguiendo; }
    bool isJugadorEnVision() const { return jugadorEnVision; }
};

#endif // MONSTRUO_H