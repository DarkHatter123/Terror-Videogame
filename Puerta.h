#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Puerta {
private:
    glm::vec3 posicion;
    float ancho;
    float alto;
    float grosor;
    float anguloActual;
    bool abierta;
    float rotacionBase;

    // Para texturas
    unsigned int texturaID;
    bool tieneTextura;
    unsigned int texturaPicaporteID;
    bool tieneTexturaPicaporte;
    bool bloqueada;
public:
    Puerta(glm::vec3 pos);
    Puerta(glm::vec3 pos, float anchoPuerta, float altoPuerta, float grosorPuerta);
    Puerta(glm::vec3 pos, float anchoPuerta, float altoPuerta, float grosorPuerta, float rotacionBaseInicial);

    void setTamanio(float nuevoAncho, float nuevoAlto, float nuevoGrosor);
    void setTextura(const char* rutaTextura);
    void setTexturaPicaporte(const char* rutaTextura);
    bool toggle(); // returns true if door actually toggled, false if blocked
    bool estaAbierta() const;
    void update(float deltaTime);
    glm::mat4 getModelMatrix();
    bool jugadorCerca(glm::vec3 jugadorPos);
    bool verificarColision(glm::vec3 jugadorPos, float radioJugador) const;
    void render(unsigned int shaderProgramID);
    void setBloqueada(bool estado);
    bool estaBloqueada() const;

    glm::vec3 getPosicion() const { return posicion; }
    bool isAbierta() const { return abierta; }

};