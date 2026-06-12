#ifndef MENU2D_H
#define MENU2D_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

#include "Shader.h"
#include <src/SOIL2/SOIL2.h>

// Variables de renderizado 2D
extern unsigned int VAO_MenuFondo, VAO_PanelIzq, VAO_PanelDer, VAO_AdornoSuperior, VAO_LineaBlanca, VAO_LineaMorada;
extern unsigned int VAO_MarcoSuperior, VAO_DecoracionEsquina, VAO_PanelInferior, VAO_PanelOscuro, VAO_DetalleAzul;
extern unsigned int VAO_BotonIzquierdo, VAO_BotonCentro, VAO_BotonDerecho, VAO_LuzCentral, VAO_Linterna;
extern unsigned int VBO_MenuFondo, VBO_PanelIzq, VBO_PanelDer, VBO_AdornoSuperior, VBO_LineaBlanca, VBO_LineaMorada;
extern unsigned int VBO_MarcoSuperior, VBO_DecoracionEsquina, VBO_PanelInferior, VBO_PanelOscuro, VBO_DetalleAzul;
extern unsigned int VBO_BotonIzquierdo, VBO_BotonCentro, VBO_BotonDerecho, VBO_LuzCentral, VBO_Linterna;
extern unsigned int EBO_MenuFondo, EBO_PanelIzq, EBO_PanelDer, EBO_AdornoSuperior, EBO_LineaBlanca, EBO_LineaMorada;
extern unsigned int EBO_MarcoSuperior, EBO_DecoracionEsquina, EBO_PanelInferior, EBO_PanelOscuro, EBO_DetalleAzul;
extern unsigned int EBO_BotonIzquierdo, EBO_BotonCentro, EBO_BotonDerecho, EBO_LuzCentral, EBO_Linterna;
extern unsigned int* vbLinternaPtr;

// Vertices de la linterna (accesible desde main.cpp)
extern GLfloat verticesLinterna[16 * 6];
extern GLuint indicesLinterna[36];

// Estados del juego
enum GameState {
    MENU_PRINCIPAL,
    PLAY,
    LOADING,
    MANUAL,
    CREDITS,
    PAUSE_MENU,
    MANUAL_FROM_PAUSE
};

extern GameState currentState;
extern bool manualVisto;
extern float tiempoMensajeBloqueo;
extern const float DURACION_MENSAJE_BLOQUEO;

// Variables para la pantalla de carga
extern std::chrono::steady_clock::time_point loadingStartTime;
extern std::chrono::steady_clock::time_point lastPuntoTime;
extern int puntosIndex;
extern bool luzBlancoAmarillentoEncendida;
extern std::chrono::steady_clock::time_point lastLuzBlink;

// Variables del parpadeo de luz central
extern bool luzBlancaEncendida;
extern std::chrono::steady_clock::time_point lastBlinkTime;

// Función para cargar texturas
unsigned int cargarTexturaMenu(const char* path);

// Función para inicializar todos los VAOs del menú 2D
void inicializarMenu2D(Shader& shaderProgram);

// Función para dibujar el menú 2D según el estado actual
void dibujarMenu2D(Shader& shaderProgram, GLuint uniID, int windowWidth, int windowHeight, float escalaImGui);

// Función para actualizar el parpadeo de la luz central
void actualizarParpadeoLuzCentral();

// Función para actualizar el parpadeo de la linterna en carga
void actualizarParpadeoLinternaCarga();

// Función para resetear los colores de la linterna
void resetearColoresLinterna();

#endif // MENU2D_H