#include "Boton.h"

Boton::Boton(const glm::vec3& pos, const glm::vec3& esc, const glm::vec3& col, int orden)
    : posicion(pos), escala(esc), colorOriginal(col),
      colorActivo(col * 1.5f),  // más brillante (o blanco: glm::vec3(1.0f))
      activo(false), ordenRequerido(orden), visible(true), radioInteraccion(1.5f) {
}

Boton::~Boton() {
}

glm::vec3 Boton::getColor() const {
    return activo ? colorActivo : colorOriginal;
}

void Boton::setActivo(bool a) {
    activo = a;
    // No cambiamos visible, solo el color
}

void Boton::reiniciar() {
    activo = false;
    visible = true;
}