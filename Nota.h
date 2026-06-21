#ifndef NOTA_H
#define NOTA_H

#include <glm/glm.hpp>
#include <string>

class Nota {
private:
    glm::vec3 posicion;
    glm::vec3 escala;
    std::string texto;
    unsigned int texturaID;
    bool visible;
    bool leida;
    float radioInteraccion;   // Nuevo

public:
    Nota(const glm::vec3& pos, const glm::vec3& esc, const std::string& txt);
    ~Nota();

    void setTextura(unsigned int texID) { texturaID = texID; }
    unsigned int getTextura() const { return texturaID; }

    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; }

    bool isLeida() const { return leida; }
    void setLeida(bool l) { leida = l; }

    const glm::vec3& getPosicion() const { return posicion; }
    const glm::vec3& getEscala() const { return escala; }
    const std::string& getTexto() const { return texto; }

    float getRadioInteraccion() const { return radioInteraccion; }
    void setRadioInteraccion(float r) { radioInteraccion = r; }
};

#endif