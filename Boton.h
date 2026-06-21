#ifndef BOTON_H
#define BOTON_H

#include <glm/glm.hpp>

class Boton {
private:
    glm::vec3 posicion;
    glm::vec3 escala;
    glm::vec3 colorOriginal;   // color cuando no está activado
    glm::vec3 colorActivo;     // color cuando está activado (ej. blanco)
    bool activo;
    int ordenRequerido;
    bool visible;
    float radioInteraccion;

public:
    Boton(const glm::vec3& pos, const glm::vec3& esc, const glm::vec3& col, int orden);
    ~Boton();

    glm::vec3 getPosicion() const { return posicion; }
    glm::vec3 getEscala() const { return escala; }
    glm::vec3 getColor() const;   // devuelve colorActual (depende de activo)
    bool isActivo() const { return activo; }
    int getOrdenRequerido() const { return ordenRequerido; }
    bool isVisible() const { return visible; }
    float getRadioInteraccion() const { return radioInteraccion; }

    void setActivo(bool a);
    void setVisible(bool v) { visible = v; }
    void setRadioInteraccion(float r) { radioInteraccion = r; }
    void reiniciar();
};

#endif