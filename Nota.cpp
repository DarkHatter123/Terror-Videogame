#include "Nota.h"

Nota::Nota(const glm::vec3& pos, const glm::vec3& esc, const std::string& txt)
    : posicion(pos), escala(esc), texto(txt), texturaID(0), visible(true), leida(false), radioInteraccion(2.0f) {
}

Nota::~Nota() {
}