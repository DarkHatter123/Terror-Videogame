#ifndef ESCENARIO_H
#define ESCENARIO_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "Model.h"
#include "Puerta.h"
#include "Lampara.h"
#include "Boton.h"
#include "Nota.h"
#include "MonstruoManager.h"

struct LuzPuntual {
    glm::vec3 posicion;
    float intensidad;
    glm::vec3 color;
    bool parpadea;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    float tiempoParpadeo;
    float offsetParpadeo;
    bool visible;
    float diffuseStrength;
};

struct ObjetoFisico {
    glm::vec3 posicion;
    glm::vec3 escala;
    glm::vec3 color;
    bool tieneColision;
    bool esPuerta;
    bool tieneTextura;
    unsigned int texturaID;
    bool rotarConPuerta;
    float rotacionY;
    bool visible;
    bool esIndicador;
    bool esMarcoPuerta;

    ObjetoFisico() :
        posicion(0.0f), escala(1.0f), color(1.0f),
        tieneColision(false), esPuerta(false), tieneTextura(false),
        texturaID(0), rotarConPuerta(false), rotacionY(0.0f),
        visible(true), esIndicador(false), esMarcoPuerta(false) {}
};

class Escenario {
private:
    Shader* shader;
    Shader* shaderLuz;
    Shader* shaderMonstruo;

    unsigned int VAO, VBO;
    unsigned int cuboVAO, cuboVBO;
    unsigned int esferaVAO, esferaVBO, esferaEBO;
    int esferaIndices;

    std::vector<ObjetoFisico> objetos;
    std::vector<LuzPuntual> luces;
    std::vector<Lampara> lamparas;
    std::vector<Puerta> puertasMadera;
    std::vector<Boton> botones;
    std::vector<Nota> notas;
    std::vector<Model*> modelosExtra;
    std::vector<glm::vec3> modelosPosiciones;
    std::vector<glm::vec3> modelosEscalas;

    // Puerta industrial
    bool puertaAbierta;
    float anguloPuerta;
    std::vector<int> objetosPuertaIndustrial;
    int indicePuertaNormal;
    glm::vec3 posicionOriginalPuerta;

    // Puzzle
    int secuenciaActual;
    bool puzzleResuelto;
    bool palancaActivable;
    bool palancaActivada;
    bool palancaAnimando;
    float anguloPalanca;
    std::vector<int> indicesIndicadoresProgreso;

    // Puertas bloqueadas
    bool puertaJefeBloqueada;
    bool puertaSalidaBloqueada;
    bool botonSalidaPresionado;

    // Texturas
    unsigned int texturaPuertaIndustrial;
    unsigned int texturaPared;
    unsigned int texturaPared2;
    unsigned int texturaPared3;
    unsigned int texturaMarcoPuerta;
    unsigned int texturaCaja1;
    unsigned int texturaCaja2;
    unsigned int texturaCaja3;
    unsigned int texturaSuelo;
    unsigned int texturaSueloJefe;
    unsigned int texturaSueloAlfombra;
    unsigned int texturaLampara;
    unsigned int texturatecho;
    unsigned int texturaBodega;
    unsigned int texturaMarcoBodega;
    unsigned int texturaNota;

    // Linterna
    struct {
        bool on;
        glm::vec3 position;
        glm::vec3 direction;
    } flashlightData;

    // Modelo palanca
    Model* modeloPalancaPtr;
    std::vector<bool> modelosTieneColision;

    // ==================== MONSTRUO ====================
    MonstruoManager* monstruoManager;
    bool monstruoActivado;
    // ================================================

    // Métodos privados
    void Bombillo();
    void Cajas();
    void Bodega();
    void Luces();
    void Puertas();
    void Estanterias();
    void PasilloBodega();
    void LucesPasillo(glm::vec3 pos);
    void PuertasMadera(glm::vec3 pos, bool rotada, bool conColision);
    void PuertaIndustrial(glm::vec3 pos, bool rotada, bool conColision);
    void LucesRectangulares(glm::vec3 pos);
    void AreaCajas();
    void PasillosAreaCajas();
    void SalaJefe();
    void AreaSeguridad();
    void PasillosAreaSeguridad();
    void Recepcion();
    void PasilloRecepcion();
    void crearIndicadoresProgreso();
    void actualizarIndicadoresProgreso();
    void configurarLuces(Shader& shader, float tiempo);
    void configurarLinterna(Shader& shader);

    // Notas
    void agregarNota(const glm::vec3& pos, const glm::vec3& esc, const std::string& texto, float radioInteraccion = 2.0f);
    void renderNotas(Shader& shader);

    // Botones
    void agregarBoton(const glm::vec3& pos, const glm::vec3& esc, const glm::vec3& col, int orden, float radio = 2.0f);
    void renderBotones(Shader& shader);
    void abrirPuertaJefe();
    void abrirPuertaSalida();

    // Monstruo
    void renderMonstruo(Shader& shader);

public:
    Escenario();
    ~Escenario();

    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, float tiempo);
    void renderPuertasMadera(const glm::mat4& view, const glm::mat4& projection, Shader* shader);
    void update(float deltaTime);

    void setFlashlight(const glm::vec3& pos, const glm::vec3& dir, bool on);
    bool verificarColisionObjetos(glm::vec3 posicionJugador, float radioJugador) const;
    bool jugadorCercaDePuerta(glm::vec3 posJugador) const;
    void togglePuerta();
    bool isPuertaAbierta() const;
    void togglePuertaMadera(glm::vec3 jugadorPos);
    bool jugadorCercaPuerta(glm::vec3 posicionJugador);
    bool jugadorCercaPalanca(glm::vec3 posJugador) const;
    void togglePalanca();
    bool isPuzzleResuelto() const { return puzzleResuelto; }

    // Notas
    bool jugadorCercaNota(const glm::vec3& posJugador, std::string& textoSalida, int& indice) const;
    void marcarNotaLeida(int indice);

    // Botones
    bool jugadorCercaBoton(const glm::vec3& posJugador, int& indice);
    void presionarBoton(int indice);

    // Metodos del moustro
    void inicializarMonstruo();
    void activarMonstruo(const glm::vec3& posicionInicial);
    void actualizarMonstruo(float deltaTime, const glm::vec3& posJugador);
    bool isJugadorAtrapado() const;
    bool isMonstruoActivo() const;
    void liberarJugador();
    void setMonstruoActivadoPorPuzzle(bool activado);
    bool isMonstruoActivadoPorPuzzle() const;
    void setMonstruoAtraviesaPuertas(bool puede);
    const Monstruo& getMonstruo() const;
    float getTiempoCongelado() const;

};

unsigned int cargarTextura(const char* path);

#endif // ESCENARIO_H