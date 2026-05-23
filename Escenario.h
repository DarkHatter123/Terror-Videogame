#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Shader.h"
#include "Lampara.h"

struct ObjetoFisico {
    glm::vec3 posicion;
    glm::vec3 escala;
    glm::vec3 color;
    bool tieneColision;
    bool esPuerta;  // Identificar si es la puerta que se abre
};

struct LuzPuntual {
    glm::vec3 posicion;
    glm::vec3 color;
    float intensidad;
    bool parpadea;
    float tiempoParpadeo;
    float offsetParpadeo;
    // Nuevos campos para foco
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
};

class Escenario {
private:
    unsigned int VAO, VBO;
    unsigned int cuboVAO, cuboVBO;
    unsigned int esferaVAO, esferaVBO, esferaEBO;
    int esferaIndices;
    Shader* shader;
    Shader* shaderLuz;

private:
    struct FlashlightData {
        glm::vec3 position;
        glm::vec3 direction;
        bool on;
    } flashlightData;

    std::vector<ObjetoFisico> objetos;
    std::vector<LuzPuntual> luces;
    std::vector<Lampara> lamparas;

    // Puerta animable
    int indicePuertaNormal;  // Índice de la puerta en el vector objetos
    bool puertaAbierta;
    float aperturaPuerta;    // 0.0 = cerrada, 1.0 = abierta
    glm::vec3 posicionOriginalPuerta;

    void setupHabitacion();
    void setupCuboUnitario();
    void setupEsfera();
    void crearEstanteriasYCajas();
    void crearLuces();
    void crearPuertas();

    // Funciones para el pasillo recto
    void crearPasilloRecto();    // Cambiado: pasillo recto en lugar de L
    void crearLuzPasillo(glm::vec3 pos);
    void crearPuertaEnPosicion(glm::vec3 pos, bool rotada, bool conColision = true);

    void crearNuevaArea();
    void crearLuzRectangular(glm::vec3 pos);
    void crearPuertaIndustrial(glm::vec3 pos, bool rotada, bool conColision);

public:
    Escenario();
    ~Escenario();
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, float tiempo);
    void update(float deltaTime);
    void configurarLuces(Shader& shader, float tiempo);

    void togglePuerta();
    bool jugadorCercaDePuerta(glm::vec3 posJugador) const;
    bool isPuertaAbierta() const { return puertaAbierta; }

    const std::vector<ObjetoFisico>& getObjetosFisicos() const { return objetos; }
    bool verificarColisionObjetos(glm::vec3 posicionJugador, float radioJugador) const;

    void setFlashlight(const glm::vec3& pos, const glm::vec3& dir, bool on);
    void configurarLinterna(Shader& shader);
};