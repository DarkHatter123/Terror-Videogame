#ifndef PUERTA_H
#define PUERTA_H

#include <glm/glm.hpp>

class Puerta {
private:
    glm::vec3 posicion;
    float ancho;
    float alto;
    float grosor;
    float anguloActual;
    bool abierta;
    float rotacionBase; //rotación base de la puerta (0 o 90 grados)

public:
    // Constructores
    Puerta(glm::vec3 pos);
    Puerta(glm::vec3 pos, float anchoPuerta, float altoPuerta, float grosorPuerta);
    Puerta(glm::vec3 pos, float anchoPuerta, float altoPuerta, float grosorPuerta, float rotacionBase);  // NUEVO

    // Métodos principales
    void update(float deltaTime);
    void render(unsigned int shaderProgramID);
    void toggle();

    // Métodos de consulta
    bool jugadorCerca(glm::vec3 jugadorPos);
    bool verificarColision(glm::vec3 jugadorPos, float radioJugador) const;
    glm::mat4 getModelMatrix();
    bool estaAbierta() const;

    // Setters
    void setTamanio(float nuevoAncho, float nuevoAlto, float nuevoGrosor);
    glm::vec3 getPosicion() const { return posicion; }
};

#endif