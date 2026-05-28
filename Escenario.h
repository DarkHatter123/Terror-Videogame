#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Shader.h"
#include "Lampara.h"
#include "Puerta.h"

struct ObjetoFisico {
    glm::vec3 posicion;
    glm::vec3 escala;
    glm::vec3 color;
    bool tieneColision;
    bool esPuerta;
    bool tieneTextura = false;      // NUEVO
    unsigned int texturaID = 0;     // NUEVO
    float rotacionY = 0.0f;
    bool rotarConPuerta = false;
};

struct LuzPuntual {
    glm::vec3 posicion;
    glm::vec3 color;
    float intensidad;
    bool parpadea;
    float tiempoParpadeo;
    float offsetParpadeo;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    bool visible; // Nota: manda a llamar si la bombilla se dibuja o no
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
private:
    unsigned int texturaPared;  // Para cargar la textura de la pared
    unsigned int texturaMarcoPuerta; // Para cargar el marco de la puerta
    unsigned int texturaPuertaIndustrial; // Para cargar la textura de la puerta

    std::vector<ObjetoFisico> objetos;
    std::vector<LuzPuntual> luces;
    std::vector<Lampara> lamparas;
    std::vector<int> objetosPuertaIndustrial;
    std::vector<Puerta> puertasMadera;
    float anguloPuerta;
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
    void crearPasillosExtensionNuevaArea();
    void crearAreaFinal1();
    void crearAreaFinal2();
    void crearPasillosDesdeAreaFinal2();
    void NuevaAreaF();

public:
    Escenario();
    ~Escenario();
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, float tiempo);
    void update(float deltaTime);
    void configurarLuces(Shader& shader, float tiempo);

    void togglePuerta();
    bool jugadorCercaDePuerta(glm::vec3 posJugador) const;
    bool isPuertaAbierta() const;
    bool jugadorCercaPuerta(glm::vec3 posicionJugador);

    const std::vector<ObjetoFisico>& getObjetosFisicos() const { return objetos; }
    bool verificarColisionObjetos(glm::vec3 posicionJugador, float radioJugador) const;

    void setFlashlight(const glm::vec3& pos, const glm::vec3& dir, bool on);
    void configurarLinterna(Shader& shader);
    void renderPuertasMadera(const glm::mat4& view, const glm::mat4& projection, Shader* shader);
    void togglePuertaMadera(glm::vec3 jugadorPos);
};