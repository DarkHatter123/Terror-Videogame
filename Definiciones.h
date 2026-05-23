#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#include <glm/glm.hpp>

struct LuzPuntual {
    glm::vec3 posicion;
    glm::vec3 color;
    float intensidad;
    bool parpadea;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    float tiempoParpadeo;
    float offsetParpadeo;
};

struct ObjetoFisico {
    glm::vec3 posicion;
    glm::vec3 escala;
    glm::vec3 color;
    bool tieneColision;
    bool esPuerta;
};

struct FlashlightData {
    glm::vec3 position;
    glm::vec3 direction;
    bool on;
};

#endif