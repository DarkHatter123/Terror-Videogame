#ifndef MONSTRUO_H
#define MONSTRUO_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
<<<<<<< HEAD
#include "Model.h"
#include <string>
=======
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3

class Monstruo {
private:
    glm::vec3 posicion;
    glm::vec3 escala;
    glm::vec3 color;
    float velocidad;
    bool activo;
    bool persiguiendo;
    bool puedeAtravesarPuertas;
    float tiempoDesdeAparicion;
    float radioColision;
    
<<<<<<< HEAD
    float rangoVision;
    float anguloVision;
    bool jugadorEnVision;
    float tiempoPerdidaVision;
    float tiempoMaximoPerdida;

    Model* modeloQuieto;
    Model* modeloCorre;

    float animTime;

=======
    // Sistema de vision
    float rangoVision;        // Distancia maxima de visin
    float anguloVision;       // Angulo de vision en grados
    bool jugadorEnVision;     // Si el jugador está en el campo de vision
    float tiempoPerdidaVision; // Tiempo que ha pasado desde que perdió de vista al jugador
    float tiempoMaximoPerdida; // Tiempo maximo que persigue sin ver al jugador

    // VAO y buffers especificos del monstruo
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int indicesCount;

    void inicializarGraficos();
<<<<<<< HEAD
=======

    // Nueva funcion para verificar visipn
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    bool puedeVerJugador(const glm::vec3& posJugador) const;

public:
    Monstruo();
    ~Monstruo();

<<<<<<< HEAD
=======
    // Metodos de inicializacion y configuracion
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    void inicializar();
    void activar(const glm::vec3& posInicial);
    void desactivar();
    void resetear();

<<<<<<< HEAD
    void actualizar(float deltaTime, const glm::vec3& posJugador);
    void render(Shader& shader);
    void setAnimacionPorDefecto(const std::string& anim) {}
    void reproducirAnimacion(const std::string& anim) {}

    bool verificarColision(const glm::vec3& posJugador, float radioJugador) const;

=======
    // Metodos de actualizaciñn
    void actualizar(float deltaTime, const glm::vec3& posJugador);
    void render(Shader& shader);

    // Metodos de verificacion
    bool verificarColision(const glm::vec3& posJugador, float radioJugador) const;

    // Getters y Setters
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    glm::vec3 getPosicion() const { return posicion; }
    void setPosicion(const glm::vec3& pos) { posicion = pos; }
    float getVelocidad() const { return velocidad; }
    void setVelocidad(float vel) { velocidad = vel; }
    bool getPuedeAtravesarPuertas() const { return puedeAtravesarPuertas; }
    void setPuedeAtravesarPuertas(bool puede) { puedeAtravesarPuertas = puede; }
    glm::vec3 getColor() const { return color; }
    void setColor(const glm::vec3& col) { color = col; }
    bool estaActivo() const { return activo; }
    bool estaPersiguiendo() const { return persiguiendo; }
    bool isJugadorEnVision() const { return jugadorEnVision; }
};

#endif // MONSTRUO_H