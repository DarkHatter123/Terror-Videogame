#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>
#include <vector>
#include <string>

// Audio SFML
#include <SFML/Audio.hpp>

// ImGui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Archivos del proyecto Proyecto_Terror (sin modificar)
#include "Skybox.h"
#include "Escenario.h"
#include "Lampara.h"
#include "Shader.h"
#include "Menu.h"
#include <SOIL2.h>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Variables para pantalla completa
bool isFullscreen = false;
int windowedPosX, windowedPosY, windowedWidth, windowedHeight;
GLFWwindow* window;

// Variables de la bodega 3D (usando clases de Proyecto_Terror)
Escenario* escenario = nullptr;
Skybox* skybox = nullptr;
bool juego3DInicializado = false;

// Variables de cámara
glm::vec3 cameraPos = glm::vec3(0.0f, -1.0f, 7.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Variables para guardar el estado de la cámara durante la pausa
glm::vec3 savedCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
float savedYaw = -90.0f;
float savedPitch = 0.0f;
bool cameraStateSaved = false;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Variable para saber si está corriendo (para sonidos)
bool estaCorriendo = false;

// Límites de la habitación
const float RADIO_JUGADOR = 0.4f;
const float ALTURA_JUGADOR = -1.0f;

// Variables globales para música
sf::Music musicaFondo;
bool musicaReproduciendo = false;

// Variables para sonido de puerta
sf::SoundBuffer bufferPuerta;
sf::Sound soundPuerta;
bool sonidoCargado = false;

// Variables para sonido de palanca
sf::SoundBuffer bufferPalanca;
sf::Sound soundPalanca;
bool sonidoPalancaCargado = false;

// Variables para pasos
sf::SoundBuffer bufferPaso1;
sf::SoundBuffer bufferPaso2;
sf::SoundBuffer bufferPaso3;
sf::Sound soundPaso;
int pasoActual = 1;
float intervaloPasos = 0.4f;
float tiempoAcumuladoPasos = 0.0f;

// Variables para el menú
Menu* menuMapa = nullptr;
bool juegoPausado = false;

// Variable para saber si el mapa ha sido obtenido
bool mapaObtenido = false;

// Variables para mensajes en pantalla
float tiempoMensajeInicial = 0.0f;
float tiempoMensajeMapaObtenido = 0.0f;
const float DURACION_MENSAJE = 4.0f;

// Estados del juego para el menú 2D
enum GameState {
    MENU_PRINCIPAL,
    PLAY,
    LOADING,
    MANUAL,
    CREDITS,
    PAUSE_MENU,
    MANUAL_FROM_PAUSE
};

GameState currentState = MENU_PRINCIPAL;
bool manualVisto = false;
float tiempoMensajeBloqueo = 0.0f;
const float DURACION_MENSAJE_BLOQUEO = 2.0f;

// Variables para la pantalla de carga
auto loadingStartTime = std::chrono::steady_clock::now();
auto lastPuntoTime = std::chrono::steady_clock::now();
int puntosIndex = 0;
bool luzBlancoAmarillentoEncendida = true;
auto lastLuzBlink = std::chrono::steady_clock::now();

// Variables del parpadeo de luz central
bool luzBlancaEncendida = true;
auto lastBlinkTime = std::chrono::steady_clock::now();

// ==================== FUNCIÓN PARA CARGAR TEXTURAS ====================
unsigned int cargarTexturaMenu(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    unsigned char* data = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);

    if (data) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(data);
        std::cout << "Textura cargada: " << path << std::endl;
    } else {
        std::cout << "Error al cargar textura: " << path << std::endl;
    }
    return textureID;
}

// ==================== VERTICES DEL MENU 2D ====================
GLfloat verticesMenuFondo[] = {
    -1.0f,-1.0f,0.0f, 0.50f,0.50f,0.50f,
     1.0f,-1.0f,0.0f, 0.50f,0.50f,0.50f,
    -0.6f, 0.6f,0.0f, 0.50f,0.50f,0.50f,
     0.6f, 0.6f,0.0f, 0.50f,0.50f,0.50f,
};
GLuint indicesMenuFondo[] = { 0,2,3, 0,3,1 };

GLfloat verticesPanelIzq[] = {
    -1.0f, -1.0f, 0.0f,   0.25f,0.25f,0.25f,
    -1.0f,  0.4f, 0.0f,   0.25f,0.25f,0.25f,
    -0.4f,  0.4f, 0.0f,   0.25f,0.25f,0.25f,
    -0.4f, -1.0f, 0.0f,   0.25f,0.25f,0.25f
};
GLuint indicesPanelIzq[] = { 0,1,2, 0,2,3 };

GLfloat verticesPanelDer[] = {
    0.4f, -1.0f, 0.0f,    0.25f,0.25f,0.25f,
    0.4f,  0.4f, 0.0f,    0.25f,0.25f,0.25f,
    1.0f,  0.4f, 0.0f,    0.25f,0.25f,0.25f,
    1.0f, -1.0f, 0.0f,    0.25f,0.25f,0.25f,
};
GLuint indicesPanelDer[] = { 0,1,2, 0,2,3 };

GLfloat verticesAdornoSuperior[] = {
    -0.8f,  0.85f, 0.0f,   0.15f, 0.15f, 0.15f,
    -0.65f,  0.4f, 0.0f,   0.15f, 0.15f, 0.15f,
    -0.60f,  0.6f, 0.0f,   0.15f, 0.15f, 0.15f,
    0.6f,  0.6f, 0.0f,     0.15f, 0.15f, 0.15f,
    0.65f,  0.4f, 0.0f,    0.15f, 0.15f, 0.15f,
    0.8f,  0.85f, 0.0f,    0.15f, 0.15f, 0.15f
};
GLuint indicesAdornoSuperior[] = { 0,1,2, 0,2,5, 3,4,5, 3,0,5, 0,1,3, 1,3,5 };

GLfloat verticesLineaBlanca[] = {
    -1.0f,  0.5f, 0.0f,   0.10f, 0.10f, 0.10f,
    -1.0f,  0.4f, 0.0f,   0.10f, 0.10f, 0.10f,
    1.0f,  0.4f, 0.0f,    0.10f, 0.10f, 0.10f,
    1.0f,  0.5f, 0.0f,    0.10f, 0.10f, 0.10f
};
GLuint indicesLineaBlanca[] = { 0,1,2, 0,2,3 };

GLfloat verticesLineaMorada[] = {
    -1.0f,  0.6f, 0.0f,   0.21f, 0.00f, 0.25f,
    -1.0f,  0.5f, 0.0f,   0.21f, 0.00f, 0.25f,
    1.0f,  0.5f, 0.0f,    0.21f, 0.00f, 0.25f,
    1.0f,  0.6f, 0.0f,    0.21f, 0.00f, 0.25f
};
GLuint indicesLineaMorada[] = { 0,1,2, 0,2,3 };

GLfloat verticesMarcoSuperior[] = {
    -0.7f,  1.0f, 0.0f,   0.34f, 0.14f, 0.39f,
    -0.8f,  0.85f, 0.0f,  0.34f, 0.14f, 0.39f,
    0.8f,  0.85f, 0.0f,   0.34f, 0.14f, 0.39f,
    0.7f,  1.0f, 0.0f,    0.34f, 0.14f, 0.39f
};
GLuint indicesMarcoSuperior[] = { 0,1,2, 0,2,3 };

GLfloat verticesDecoracionEsquina[] = {
    0.95f,  1.0f, 0.0f,    1.0f, 1.0f, 1.0f,
    0.90f,  0.92f, 0.0f,   1.0f, 1.0f, 1.0f,
    0.925f, 0.84f, 0.0f,   1.0f, 1.0f, 1.0f,
    0.96f,  0.75f, 0.0f,   1.0f, 1.0f, 1.0f,
    0.98f,  0.88f, 0.0f,   1.0f, 1.0f, 1.0f,
    0.96f,  0.84f, 0.0f,   1.0f, 1.0f, 1.0f,
    0.93f,  0.92f, 0.0f,   1.0f, 1.0f, 1.0f
};
GLuint indicesDecoracionEsquina[] = { 0,1,6, 1,6,2, 2,6,3, 3,6,5, 3,5,4 };

GLfloat verticesPanelInferior[] = {
    -0.8f, -1.0f, 0.0f,   0.16f, 0.16f, 0.16f,
    0.8f, -1.0f, 0.0f,    0.16f, 0.16f, 0.16f,
    -0.5f,  0.35f, 0.0f,  0.16f, 0.16f, 0.16f,
    0.5f,  0.35f, 0.0f,   0.16f, 0.16f, 0.16f
};
GLuint indicesPanelInferior[] = { 0,2,3, 0,3,1 };

GLfloat verticesPanelOscuro[] = {
    -0.7f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    0.7f, -1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
    -0.45f,  0.28f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.45f,  0.28f, 0.0f,  0.0f, 0.0f, 0.0f,
};
GLuint indicesPanelOscuro[] = { 0,2,3, 0,3,1 };

GLfloat verticesDetalleAzul[] = {
    -0.45f, 0.28f, 0.0f,   0.3f, 0.5f, 0.7f,
    0.45f, 0.28f, 0.0f,    0.3f, 0.5f, 0.7f,
    -0.488f,  0.10f, 0.0f, 0.3f, 0.5f, 0.7f,
    0.488f,  0.10f, 0.0f,  0.3f, 0.5f, 0.7f,
};
GLuint indicesDetalleAzul[] = { 0,2,3, 0,3,1 };

GLfloat verticesBotonIzquierdo[] = {
    -0.40f, 0.24f, 0.0f,   0.098f, 0.098f, 0.439f,
    -0.17f, 0.24f, 0.0f,   0.098f, 0.098f, 0.439f,
    -0.438f, 0.13f, 0.0f,  0.098f, 0.098f, 0.439f,
    -0.208f, 0.13f, 0.0f,  0.098f, 0.098f, 0.439f
};
GLuint indicesBotonIzquierdo[] = { 0,2,3, 0,3,1 };

GLfloat verticesBotonCentro[] = {
    -0.12f, 0.24f, 0.0f,    0.098f, 0.098f, 0.439f,
    0.11f, 0.24f, 0.0f,     0.098f, 0.098f, 0.439f,
    -0.158f, 0.13f, 0.0f,   0.098f, 0.098f, 0.439f,
    0.148f, 0.13f, 0.0f,    0.098f, 0.098f, 0.439f,
};
GLuint indicesBotonCentro[] = { 0,2,3, 0,3,1 };

GLfloat verticesBotonDerecho[] = {
    0.16f, 0.24f, 0.0f,    0.098f, 0.098f, 0.439f,
    0.40f, 0.24f, 0.0f,    0.098f, 0.098f, 0.439f,
    0.198f, 0.13f, 0.0f,   0.098f, 0.098f, 0.439f,
    0.438f, 0.13f, 0.0f,   0.098f, 0.098f, 0.439f,
};
GLuint indicesBotonDerecho[] = { 0,2,3, 0,3,1 };

GLfloat verticesLuzCentral[] = {
    -0.45f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
     0.45f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
    -0.10f,  0.10f, 0.0f,  1.0f, 1.0f, 1.0f,
     0.10f,  0.10f, 0.0f,  1.0f, 1.0f, 1.0f,
};
GLuint indicesLuzCentral[] = { 0,2,3, 0,3,1 };

// Vertices de la linterna para carga (parpadea)
GLfloat verticesLinterna[] = {
    0.25f, -0.3f, 0.0f,   0.4f, 0.4f, 0.4f,
    0.05f, -0.3f, 0.0f,   0.4f, 0.4f, 0.4f,
    -0.05f, 0.2f, 0.0f,   0.4f, 0.4f, 0.4f,
    -0.5f, 0.2f, 0.0f,   0.4f, 0.4f, 0.4f,
    -0.5f, -0.2f, 0.0f,   0.4f, 0.4f, 0.4f,
    -0.05f, -0.2f, 0.0f,   0.4f, 0.4f, 0.4f,
    0.05f, 0.3f, 0.0f,   0.4f, 0.4f, 0.4f,
    0.25f, 0.3f, 0.0f,   0.4f, 0.4f, 0.4f,
    0.15f, -0.2f, 0.0f,   0.9f, 0.8f, 0.2f,
    0.25f, -0.2f, 0.0f,   0.9f, 0.8f, 0.2f,
    0.15f,  0.2f, 0.0f,   0.9f, 0.8f, 0.2f,
    0.25f,  0.2f, 0.0f,   0.9f, 0.8f, 0.2f,
    0.25f, -0.2f, 0.0f,   1.0f, 0.95f, 0.7f,
    1.0f, -0.5f, 0.0f,    1.0f, 0.95f, 0.7f,
    0.25f,  0.2f, 0.0f,   1.0f, 0.95f, 0.7f,
    1.0f,  0.5f, 0.0f,    1.0f, 0.95f, 0.7f,
};
GLuint indicesLinterna[] = {
    0,1,2, 1,2,3, 2,3,4, 2,4,5,
    5,6,7, 5,7,0, 2,6,7, 2,5,7,
    8,9,10, 9,10,11,
    12,13,14, 13,14,15
};

// Variables de renderizado 2D
unsigned int VAO_MenuFondo, VAO_PanelIzq, VAO_PanelDer, VAO_AdornoSuperior, VAO_LineaBlanca, VAO_LineaMorada;
unsigned int VAO_MarcoSuperior, VAO_DecoracionEsquina, VAO_PanelInferior, VAO_PanelOscuro, VAO_DetalleAzul;
unsigned int VAO_BotonIzquierdo, VAO_BotonCentro, VAO_BotonDerecho, VAO_LuzCentral, VAO_Linterna;
unsigned int VBO_MenuFondo, VBO_PanelIzq, VBO_PanelDer, VBO_AdornoSuperior, VBO_LineaBlanca, VBO_LineaMorada;
unsigned int VBO_MarcoSuperior, VBO_DecoracionEsquina, VBO_PanelInferior, VBO_PanelOscuro, VBO_DetalleAzul;
unsigned int VBO_BotonIzquierdo, VBO_BotonCentro, VBO_BotonDerecho, VBO_LuzCentral, VBO_Linterna;
unsigned int EBO_MenuFondo, EBO_PanelIzq, EBO_PanelDer, EBO_AdornoSuperior, EBO_LineaBlanca, EBO_LineaMorada;
unsigned int EBO_MarcoSuperior, EBO_DecoracionEsquina, EBO_PanelInferior, EBO_PanelOscuro, EBO_DetalleAzul;
unsigned int EBO_BotonIzquierdo, EBO_BotonCentro, EBO_BotonDerecho, EBO_LuzCentral, EBO_Linterna;
unsigned int* vbLinternaPtr = nullptr;

// ==================== FUNCIONES DE COLISIÓN DE LA BODEGA ====================
const float HABITACION_ANCHO = 20.0f;
const float HABITACION_ALTO = 10.0f;
const float HABITACION_PROFUNDO = 30.0f;

bool verificarColisionParedes(glm::vec3 nuevaPos) {
    // --- ZONA NUEVA: ÁREA CON DIVISIÓN (Z de 56.0 a 74.0) ---
    if (nuevaPos.z > 56.0f && nuevaPos.z <= 74.0f && nuevaPos.x > 9.0f) {
        float xMin = 9.5f + RADIO_JUGADOR;
        float xMax = 27.5f - RADIO_JUGADOR;
        float zFin = 74.0f - RADIO_JUGADOR;

        if (nuevaPos.x < xMin || nuevaPos.x > xMax) return false;

        bool enPuertaFrente = (nuevaPos.z >= zFin && nuevaPos.x > 17.4f && nuevaPos.x < 19.6f);
        if (nuevaPos.z > zFin && !enPuertaFrente) return false;

        if (nuevaPos.z > 58.0f + RADIO_JUGADOR) {
            float paredDivX = 15.5f;
            float margenPared = 0.05f + RADIO_JUGADOR;
            if (nuevaPos.x > paredDivX - margenPared && nuevaPos.x < paredDivX + margenPared) {
                return false;
            }
        }
        return true;
    }

    // --- ZONA 9: PASILLO FRONTAL ---
    if (nuevaPos.x > 27.5f) {
        float xMax = 37.5f - RADIO_JUGADOR;
        float zMin = 35.5f + RADIO_JUGADOR;
        float zMax = 38.5f - RADIO_JUGADOR;

        if (nuevaPos.x > xMax) return false;
        if (nuevaPos.z < zMin) return false;
        if (nuevaPos.z > zMax) return false;

        return true;
    }

    // --- ZONA 8: PASILLO TRASERO ---
    if (nuevaPos.z > 46.0f && nuevaPos.x > 9.5f && nuevaPos.z <= 56.0f) {
        float zMax = 56.0f - RADIO_JUGADOR;
        float xMin = 17.0f + RADIO_JUGADOR;
        float xMax = 20.0f - RADIO_JUGADOR;

        if (nuevaPos.x < xMin || nuevaPos.x > xMax) return false;

        bool enPuertaFondo = (nuevaPos.z >= zMax && nuevaPos.x > 17.4f && nuevaPos.x < 19.6f);
        if (nuevaPos.z > zMax && !enPuertaFondo) return false;

        return true;
    }

    // --- ZONA 7: ÁREA FINAL 2 ---
    if (nuevaPos.x > 9.5f && nuevaPos.z >= 28.0f && nuevaPos.z <= 46.0f) {
        float xMin = 9.5f + RADIO_JUGADOR;
        float xMax = 27.5f - RADIO_JUGADOR;
        float zMin = 28.0f + RADIO_JUGADOR;
        float zMax = 46.0f - RADIO_JUGADOR;

        bool enPuertaIzquierda = (nuevaPos.x <= xMin && nuevaPos.z > 35.5f && nuevaPos.z < 38.5f);
        bool enPuertaFrente = (nuevaPos.x >= xMax && nuevaPos.z > 35.5f && nuevaPos.z < 38.5f);
        bool enPuertaDer = (nuevaPos.z <= zMin && nuevaPos.x > 17.0f && nuevaPos.x < 20.0f);
        bool enPuertaAtras = (nuevaPos.z >= zMax && nuevaPos.x > 17.0f && nuevaPos.x < 20.0f);

        if (nuevaPos.x > xMax && !enPuertaFrente) return false;
        if (nuevaPos.x < xMin && !enPuertaIzquierda) return false;
        if (nuevaPos.z > zMax && !enPuertaAtras) return false;
        if (nuevaPos.z < zMin && !enPuertaDer) return false;
        return true;
    }

    // --- ZONA 6: ÁREA FINAL 1 ---
    if (nuevaPos.z > 54.0f && nuevaPos.x <= 9.5f) {
        float xMin = -14.5f + RADIO_JUGADOR;
        float xMax = -0.5f - RADIO_JUGADOR;
        float zMin = 54.0f + RADIO_JUGADOR;
        float zMax = 68.0f - RADIO_JUGADOR;

        bool enPuertaAtras = (nuevaPos.z <= zMin && nuevaPos.x > -9.0f && nuevaPos.x < -6.0f);

        if (nuevaPos.x < xMin || nuevaPos.x > xMax) return false;
        if (nuevaPos.z > zMax) return false;
        if (nuevaPos.z < zMin && !enPuertaAtras) return false;
        return true;
    }

    // --- ZONA 4: PASILLO EXTENSIÓN 1 ---
    if (nuevaPos.z > 44.0f && nuevaPos.z <= 54.0f && nuevaPos.x <= 9.5f) {
        if (nuevaPos.x < -9.0f + RADIO_JUGADOR || nuevaPos.x > -6.0f - RADIO_JUGADOR) return false;
        return true;
    }

    // --- ZONA 3 Y 5: NUEVA ÁREA Y PASILLO DERECHO ---
    if (nuevaPos.z > 30.0f && nuevaPos.z <= 44.0f) {
        if (nuevaPos.x > -0.5f && nuevaPos.x <= 9.5f) {
            if (nuevaPos.z < 35.5f + RADIO_JUGADOR || nuevaPos.z > 38.5f - RADIO_JUGADOR) return false;
            return true;
        }

        float xMinSala = -14.5f + RADIO_JUGADOR;
        float xMaxSala = -0.5f - RADIO_JUGADOR;
        float zMaxSala = 44.0f - RADIO_JUGADOR;

        if (nuevaPos.x < xMinSala) return false;
        if (nuevaPos.x > xMaxSala && (nuevaPos.z < 35.5f || nuevaPos.z > 38.5f)) return false;
        if (nuevaPos.z > zMaxSala && (nuevaPos.x < -8.5f || nuevaPos.x > -6.5f)) return false;
        return true;
    }

    // --- ZONA 2: PASILLO RECTO ---
    if (nuevaPos.z > 15.0f && nuevaPos.z <= 30.0f) {
        if (nuevaPos.x < -8.8f || nuevaPos.x > -6.2f) return false;
        if (nuevaPos.z > 29.5f && (nuevaPos.x < -8.5f || nuevaPos.x > -6.5f)) return false;
        return true;
    }

    // --- ZONA 1: BODEGA PRINCIPAL ---
    if (nuevaPos.z >= -15.0f && nuevaPos.z <= 15.0f) {
        if (nuevaPos.x < -9.5f || nuevaPos.x > 9.5f) return false;
        if (nuevaPos.z < -14.5f) return false;
        if (nuevaPos.z > 14.5f && (nuevaPos.x < -8.8f || nuevaPos.x > -6.2f)) return false;
        return true;
    }

    return false;
}

void reproducirPaso() {
    if (estaCorriendo) {
        if (pasoActual == 1) {
            soundPaso.setBuffer(bufferPaso1);
            pasoActual = 2;
        } else if (pasoActual == 2) {
            soundPaso.setBuffer(bufferPaso2);
            pasoActual = 3;
        } else {
            soundPaso.setBuffer(bufferPaso3);
            pasoActual = 1;
        }
        soundPaso.setVolume(55.0f);
    } else {
        if (pasoActual == 1) {
            soundPaso.setBuffer(bufferPaso1);
            pasoActual = 2;
        } else if (pasoActual == 2) {
            soundPaso.setBuffer(bufferPaso2);
            pasoActual = 3;
        } else {
            soundPaso.setBuffer(bufferPaso3);
            pasoActual = 1;
        }
        soundPaso.setVolume(40.0f);
    }
    soundPaso.play();
}

bool eventoSalidaActivado = false;
bool puertaBloqueada = false;

bool jugadorEnZonaSalida(glm::vec3 pos) {
    return (pos.x > -9.0f && pos.x < -6.0f && pos.z > 28.0f && pos.z < 31.5f);
}

// ==================== FUNCIÓN PARA ALTERNAR PANTALLA COMPLETA ====================
void toggleFullscreen(GLFWwindow* window) {
    static bool isFullscreen = false;
    static int savedX, savedY, savedW, savedH;

    if (isFullscreen) {
        glfwSetWindowMonitor(window, NULL, savedX, savedY, savedW, savedH, 0);
        isFullscreen = false;
    } else {
        glfwGetWindowPos(window, &savedX, &savedY);
        glfwGetWindowSize(window, &savedW, &savedH);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        isFullscreen = true;
    }
}

// ==================== FUNCIÓN PARA REINICIAR EL JUEGO ====================
void reiniciarJuego() {
    if (juego3DInicializado) {
        delete escenario;
        delete skybox;
        escenario = nullptr;
        skybox = nullptr;
        juego3DInicializado = false;
    }
    musicaFondo.stop();
    musicaReproduciendo = false;
    mapaObtenido = false;
    eventoSalidaActivado = false;
    puertaBloqueada = false;
    juegoPausado = false;
    if (menuMapa) {
        menuMapa->disable();
        menuMapa->setVisible(false);
    }
    cameraPos = glm::vec3(0.0f, ALTURA_JUGADOR, 7.5f);
    yaw = -90.0f;
    pitch = 0.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    firstMouse = true;
    tiempoMensajeInicial = DURACION_MENSAJE;
    tiempoMensajeMapaObtenido = 0.0f;
}

// ==================== PROCESAMIENTO DE INPUT ====================
void processInput(GLFWwindow* window, float deltaTime) {
    // ===== TECLAS QUE SIEMPRE FUNCIONAN (incluso en pausa/mapa) =====
    static bool lastGState = false;
    bool currentGState = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
    if (currentGState && !lastGState) {
        static bool fullscreen = false;
        static int savedX, savedY, savedW, savedH;

        if (fullscreen) {
            glfwSetWindowMonitor(window, NULL, savedX, savedY, savedW, savedH, 0);
            fullscreen = false;
            std::cout << "Ventana mode" << std::endl;
        } else {
            glfwGetWindowPos(window, &savedX, &savedY);
            glfwGetWindowSize(window, &savedW, &savedH);
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            fullscreen = true;
            std::cout << "Fullscreen mode" << std::endl;
        }
    }
    lastGState = currentGState;

    static bool lastZState = false;
    bool currentZState = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
    if (currentZState && !lastZState && menuMapa && menuMapa->isVisible()) {
        juegoPausado = false;
        menuMapa->setVisible(false);
        if (cameraStateSaved) {
            cameraFront = savedCameraFront;
            yaw = savedYaw;
            pitch = savedPitch;
            cameraStateSaved = false;
        }
        glfwSetCursorPos(window, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f);
        lastX = SCR_WIDTH / 2.0f;
        lastY = SCR_HEIGHT / 2.0f;
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        std::cout << "Mapa cerrado (Z)" << std::endl;
    }
    lastZState = currentZState;

    if (juegoPausado || (menuMapa && menuMapa->isVisible())) {
        return;
    }

    if (currentState == PLAY) {
        float cameraSpeed = 2.5f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            cameraSpeed = 5.0f * deltaTime;
        }

        glm::vec3 nuevaPos = cameraPos;
        bool mover = false;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            glm::vec3 dir = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
            nuevaPos += cameraSpeed * dir;
            mover = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            glm::vec3 dir = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
            nuevaPos -= cameraSpeed * dir;
            mover = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm::vec3 derecha = glm::normalize(glm::cross(cameraFront, cameraUp));
            glm::vec3 derechaH = glm::normalize(glm::vec3(derecha.x, 0.0f, derecha.z));
            nuevaPos -= derechaH * cameraSpeed;
            mover = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm::vec3 derecha = glm::normalize(glm::cross(cameraFront, cameraUp));
            glm::vec3 derechaH = glm::normalize(glm::vec3(derecha.x, 0.0f, derecha.z));
            nuevaPos += derechaH * cameraSpeed;
            mover = true;
        }

        if (mover) {
            if (verificarColisionParedes(nuevaPos) && escenario && !escenario->verificarColisionObjetos(nuevaPos, RADIO_JUGADOR)) {
                cameraPos = nuevaPos;
                cameraPos.y = ALTURA_JUGADOR;
            }
            tiempoAcumuladoPasos += deltaTime;
            estaCorriendo = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
            float intervaloActual = estaCorriendo ? 0.2f : 0.4f;
            if (tiempoAcumuladoPasos >= intervaloActual) {
                tiempoAcumuladoPasos = 0.0f;
                reproducirPaso();
            }
        } else {
            tiempoAcumuladoPasos = 0.0f;
            estaCorriendo = false;
        }

        // Tecla F: linterna
        static bool lastFState = false;
        static bool flashlightOn = true;
        bool currentFState = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
        if (currentFState && !lastFState) {
            flashlightOn = !flashlightOn;
        }
        lastFState = currentFState;
        if (escenario) escenario->setFlashlight(cameraPos, cameraFront, flashlightOn);

        // Tecla E: interactuar
        static bool ePresionada = false;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !ePresionada) {
            if (escenario) escenario->togglePuertaMadera(cameraPos);
            if (escenario && escenario->jugadorCercaDePuerta(cameraPos)) {
                escenario->togglePuerta();
                if (sonidoCargado) soundPuerta.play();
            }
            if (escenario && escenario->jugadorCercaPalanca(cameraPos)) {
                escenario->togglePalanca();
                if (sonidoPalancaCargado) soundPalanca.play();
                if (escenario->isPalancaActivada()) std::cout << "¡Palanca activada!" << std::endl;
            }
            ePresionada = true;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
            ePresionada = false;
        }

        // Tecla M: ABRIR mapa
        static bool lastMState = false;
        bool currentMState = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
        if (currentMState && !lastMState && mapaObtenido && menuMapa && !menuMapa->isVisible() && !juegoPausado) {
            juegoPausado = true;
            menuMapa->setVisible(true);
            savedCameraFront = cameraFront;
            savedYaw = yaw;
            savedPitch = pitch;
            cameraStateSaved = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            std::cout << "Mapa abierto (M)" << std::endl;
        }
        lastMState = currentMState;

        // Tecla T: pausar música
        static bool lastTState = false;
        bool currentTState = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
        if (currentTState && !lastTState) {
            if (musicaReproduciendo) {
                musicaFondo.pause();
                musicaReproduciendo = false;
            } else {
                musicaFondo.play();
                musicaReproduciendo = true;
            }
        }
        lastTState = currentTState;
    }

    // ESC: manejar según el estado
    static bool lastEscState = false;
    bool currentEscState = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (currentEscState && !lastEscState) {
        if (currentState == PLAY && !juegoPausado) {
            currentState = PAUSE_MENU;
            juegoPausado = true;
            savedCameraFront = cameraFront;
            savedYaw = yaw;
            savedPitch = pitch;
            cameraStateSaved = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            std::cout << "Menú de pausa abierto" << std::endl;
        }
    }
    lastEscState = currentEscState;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (juegoPausado || currentState != PLAY) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (juegoPausado || currentState != PLAY) return;
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ==================== FUNCIÓN PARA INICIALIZAR LA BODEGA ====================
void inicializarBodega() {
    if (juego3DInicializado) return;

    std::cout << "=== INICIALIZANDO BODEGA 3D ===" << std::endl;

    if (musicaFondo.openFromFile("sounds/Ambience/Ambient_sound.ogg")) {
        musicaFondo.setLoop(true);
        musicaFondo.setVolume(50.0f);
        musicaFondo.play();
        musicaReproduciendo = true;
        std::cout << "Música cargada" << std::endl;
    }

    if (bufferPaso1.loadFromFile("sounds/SFX/Pasos_1.wav")) std::cout << "Paso 1 cargado" << std::endl;
    if (bufferPaso2.loadFromFile("sounds/SFX/Pasos_2.wav")) std::cout << "Paso 2 cargado" << std::endl;
    if (bufferPaso3.loadFromFile("sounds/SFX/Pasos_3.wav")) std::cout << "Paso 3 cargado" << std::endl;
    if (bufferPuerta.loadFromFile("sounds/SFX/puerta.wav")) {
        soundPuerta.setBuffer(bufferPuerta);
        sonidoCargado = true;
        soundPuerta.setVolume(60.0f);
        std::cout << "Sonido de puerta cargado" << std::endl;
    }
    if (bufferPalanca.loadFromFile("sounds/SFX/palanca.wav")) {
        soundPalanca.setBuffer(bufferPalanca);
        sonidoPalancaCargado = true;
        soundPalanca.setVolume(60.0f);
        std::cout << "Sonido de palanca cargado" << std::endl;
    }

    escenario = new Escenario();
    std::vector<std::string> faces;
    faces.push_back("Textures/px.jpg");
    faces.push_back("Textures/nx.jpg");
    faces.push_back("Textures/py.jpg");
    faces.push_back("Textures/ny.jpg");
    faces.push_back("Textures/nz.jpg");
    faces.push_back("Textures/pz.jpg");
    skybox = new Skybox(faces);

    cameraPos = glm::vec3(0.0f, ALTURA_JUGADOR, 7.5f);
    yaw = -90.0f;
    pitch = 0.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    firstMouse = true;

    juego3DInicializado = true;
    eventoSalidaActivado = false;
    puertaBloqueada = false;
    mapaObtenido = false;
    if (menuMapa) {
        menuMapa->disable();
        menuMapa->setVisible(false);
    }
    tiempoMensajeInicial = DURACION_MENSAJE;
    tiempoMensajeMapaObtenido = 0.0f;

    std::cout << "Bodega inicializada correctamente" << std::endl;
}

int main() {
    std::cout << "=== INICIANDO KENNY'S WAREHOUSE ===" << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "UNSTABLE ANOMALY", NULL, NULL);
    if (!window) {
        std::cout << "ERROR: No se pudo crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR: No se pudo inicializar GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ========== INICIALIZAR IMGUI ==========
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.6f;

    // ========== CARGAR TEXTURA DEL TÍTULO ==========
    unsigned int texturaTituloID = cargarTexturaMenu("Unstable.png");
    bool texturaCargada = (texturaTituloID != 0);

    // ========== INICIALIZAR SHADER 2D ==========
    Shader shaderProgram("default.vert", "default.frag");
    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    // ========== CONFIGURAR TODOS LOS VAOs DEL MENU 2D ==========
    // VAO_MenuFondo
    glGenVertexArrays(1, &VAO_MenuFondo);
    glGenBuffers(1, &VBO_MenuFondo);
    glGenBuffers(1, &EBO_MenuFondo);
    glBindVertexArray(VAO_MenuFondo);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_MenuFondo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMenuFondo), verticesMenuFondo, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_MenuFondo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesMenuFondo), indicesMenuFondo, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_PanelIzq
    glGenVertexArrays(1, &VAO_PanelIzq);
    glGenBuffers(1, &VBO_PanelIzq);
    glGenBuffers(1, &EBO_PanelIzq);
    glBindVertexArray(VAO_PanelIzq);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PanelIzq);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPanelIzq), verticesPanelIzq, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_PanelIzq);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPanelIzq), indicesPanelIzq, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_PanelDer
    glGenVertexArrays(1, &VAO_PanelDer);
    glGenBuffers(1, &VBO_PanelDer);
    glGenBuffers(1, &EBO_PanelDer);
    glBindVertexArray(VAO_PanelDer);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PanelDer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPanelDer), verticesPanelDer, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_PanelDer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPanelDer), indicesPanelDer, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_AdornoSuperior
    glGenVertexArrays(1, &VAO_AdornoSuperior);
    glGenBuffers(1, &VBO_AdornoSuperior);
    glGenBuffers(1, &EBO_AdornoSuperior);
    glBindVertexArray(VAO_AdornoSuperior);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_AdornoSuperior);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAdornoSuperior), verticesAdornoSuperior, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_AdornoSuperior);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesAdornoSuperior), indicesAdornoSuperior, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_LineaBlanca
    glGenVertexArrays(1, &VAO_LineaBlanca);
    glGenBuffers(1, &VBO_LineaBlanca);
    glGenBuffers(1, &EBO_LineaBlanca);
    glBindVertexArray(VAO_LineaBlanca);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_LineaBlanca);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLineaBlanca), verticesLineaBlanca, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LineaBlanca);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesLineaBlanca), indicesLineaBlanca, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_LineaMorada
    glGenVertexArrays(1, &VAO_LineaMorada);
    glGenBuffers(1, &VBO_LineaMorada);
    glGenBuffers(1, &EBO_LineaMorada);
    glBindVertexArray(VAO_LineaMorada);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_LineaMorada);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLineaMorada), verticesLineaMorada, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LineaMorada);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesLineaMorada), indicesLineaMorada, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_MarcoSuperior
    glGenVertexArrays(1, &VAO_MarcoSuperior);
    glGenBuffers(1, &VBO_MarcoSuperior);
    glGenBuffers(1, &EBO_MarcoSuperior);
    glBindVertexArray(VAO_MarcoSuperior);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_MarcoSuperior);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMarcoSuperior), verticesMarcoSuperior, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_MarcoSuperior);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesMarcoSuperior), indicesMarcoSuperior, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_DecoracionEsquina
    glGenVertexArrays(1, &VAO_DecoracionEsquina);
    glGenBuffers(1, &VBO_DecoracionEsquina);
    glGenBuffers(1, &EBO_DecoracionEsquina);
    glBindVertexArray(VAO_DecoracionEsquina);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_DecoracionEsquina);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDecoracionEsquina), verticesDecoracionEsquina, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_DecoracionEsquina);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesDecoracionEsquina), indicesDecoracionEsquina, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_PanelInferior
    glGenVertexArrays(1, &VAO_PanelInferior);
    glGenBuffers(1, &VBO_PanelInferior);
    glGenBuffers(1, &EBO_PanelInferior);
    glBindVertexArray(VAO_PanelInferior);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PanelInferior);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPanelInferior), verticesPanelInferior, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_PanelInferior);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPanelInferior), indicesPanelInferior, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_PanelOscuro
    glGenVertexArrays(1, &VAO_PanelOscuro);
    glGenBuffers(1, &VBO_PanelOscuro);
    glGenBuffers(1, &EBO_PanelOscuro);
    glBindVertexArray(VAO_PanelOscuro);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PanelOscuro);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPanelOscuro), verticesPanelOscuro, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_PanelOscuro);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPanelOscuro), indicesPanelOscuro, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_DetalleAzul
    glGenVertexArrays(1, &VAO_DetalleAzul);
    glGenBuffers(1, &VBO_DetalleAzul);
    glGenBuffers(1, &EBO_DetalleAzul);
    glBindVertexArray(VAO_DetalleAzul);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_DetalleAzul);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDetalleAzul), verticesDetalleAzul, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_DetalleAzul);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesDetalleAzul), indicesDetalleAzul, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_BotonIzquierdo
    glGenVertexArrays(1, &VAO_BotonIzquierdo);
    glGenBuffers(1, &VBO_BotonIzquierdo);
    glGenBuffers(1, &EBO_BotonIzquierdo);
    glBindVertexArray(VAO_BotonIzquierdo);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_BotonIzquierdo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBotonIzquierdo), verticesBotonIzquierdo, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BotonIzquierdo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBotonIzquierdo), indicesBotonIzquierdo, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_BotonCentro
    glGenVertexArrays(1, &VAO_BotonCentro);
    glGenBuffers(1, &VBO_BotonCentro);
    glGenBuffers(1, &EBO_BotonCentro);
    glBindVertexArray(VAO_BotonCentro);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_BotonCentro);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBotonCentro), verticesBotonCentro, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BotonCentro);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBotonCentro), indicesBotonCentro, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_BotonDerecho
    glGenVertexArrays(1, &VAO_BotonDerecho);
    glGenBuffers(1, &VBO_BotonDerecho);
    glGenBuffers(1, &EBO_BotonDerecho);
    glBindVertexArray(VAO_BotonDerecho);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_BotonDerecho);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBotonDerecho), verticesBotonDerecho, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BotonDerecho);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBotonDerecho), indicesBotonDerecho, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_LuzCentral
    glGenVertexArrays(1, &VAO_LuzCentral);
    glGenBuffers(1, &VBO_LuzCentral);
    glGenBuffers(1, &EBO_LuzCentral);
    glBindVertexArray(VAO_LuzCentral);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_LuzCentral);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLuzCentral), verticesLuzCentral, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LuzCentral);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesLuzCentral), indicesLuzCentral, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO_Linterna
    glGenVertexArrays(1, &VAO_Linterna);
    glGenBuffers(1, &VBO_Linterna);
    glGenBuffers(1, &EBO_Linterna);
    glBindVertexArray(VAO_Linterna);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Linterna);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLinterna), verticesLinterna, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Linterna);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesLinterna), indicesLinterna, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    vbLinternaPtr = &VBO_Linterna;

    // ========== INICIALIZAR MENÚ DEL MAPA ==========
    menuMapa = new Menu();
    if (!menuMapa->loadTexture("Textures/mapa.png")) {
        std::cout << "Advertencia: No se pudo cargar la textura del mapa" << std::endl;
    }
    menuMapa->disable();
    menuMapa->setVisible(false);

    // Inicializar timers
    lastFrame = 0.0f;
    tiempoMensajeInicial = DURACION_MENSAJE;

    // ========== BUCLE PRINCIPAL ==========
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        auto now = std::chrono::steady_clock::now();

        if (tiempoMensajeBloqueo > 0.0f) {
            tiempoMensajeBloqueo -= deltaTime;
            if (tiempoMensajeBloqueo < 0.0f) tiempoMensajeBloqueo = 0.0f;
        }

        if (currentState == PLAY) {
            if (tiempoMensajeInicial > 0.0f) {
                tiempoMensajeInicial -= deltaTime;
                if (tiempoMensajeInicial < 0.0f) tiempoMensajeInicial = 0.0f;
            }
            if (tiempoMensajeMapaObtenido > 0.0f) {
                tiempoMensajeMapaObtenido -= deltaTime;
                if (tiempoMensajeMapaObtenido < 0.0f) tiempoMensajeMapaObtenido = 0.0f;
            }
        }

        // Parpadeo de la luz central (solo en menús 2D)
        if (currentState != PLAY && currentState != LOADING && currentState != PAUSE_MENU && currentState != MANUAL_FROM_PAUSE) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlinkTime);
            if (elapsed.count() >= 1000) {
                luzBlancaEncendida = !luzBlancaEncendida;
                lastBlinkTime = now;
                float colorValue = luzBlancaEncendida ? 1.0f : 0.0f;
                GLfloat nuevosColores[] = {
                    -0.45f, -1.0f, 0.0f,   colorValue, colorValue, colorValue,
                     0.45f, -1.0f, 0.0f,   colorValue, colorValue, colorValue,
                    -0.10f,  0.10f, 0.0f,  colorValue, colorValue, colorValue,
                     0.10f,  0.10f, 0.0f,  colorValue, colorValue, colorValue,
                };
                glBindBuffer(GL_ARRAY_BUFFER, VBO_LuzCentral);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(nuevosColores), nuevosColores);
            }
        }

        // Lógica de la pantalla de LOADING
        if (currentState == LOADING) {
            auto puntoElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPuntoTime);
            if (puntoElapsed.count() >= 500) {
                puntosIndex = (puntosIndex + 1) % 4;
                lastPuntoTime = now;
            }
            auto luzBlinkElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLuzBlink);
            if (luzBlinkElapsed.count() >= 300) {
                luzBlancoAmarillentoEncendida = !luzBlancoAmarillentoEncendida;
                lastLuzBlink = now;
                if (vbLinternaPtr) {
                    glBindBuffer(GL_ARRAY_BUFFER, *vbLinternaPtr);
                    if (luzBlancoAmarillentoEncendida) {
                        verticesLinterna[12*6 + 3] = 1.0f; verticesLinterna[12*6 + 4] = 0.95f; verticesLinterna[12*6 + 5] = 0.7f;
                        verticesLinterna[13*6 + 3] = 1.0f; verticesLinterna[13*6 + 4] = 0.95f; verticesLinterna[13*6 + 5] = 0.7f;
                        verticesLinterna[14*6 + 3] = 1.0f; verticesLinterna[14*6 + 4] = 0.95f; verticesLinterna[14*6 + 5] = 0.7f;
                        verticesLinterna[15*6 + 3] = 1.0f; verticesLinterna[15*6 + 4] = 0.95f; verticesLinterna[15*6 + 5] = 0.7f;
                    } else {
                        verticesLinterna[12*6 + 3] = 0.0f; verticesLinterna[12*6 + 4] = 0.0f; verticesLinterna[12*6 + 5] = 0.0f;
                        verticesLinterna[13*6 + 3] = 0.0f; verticesLinterna[13*6 + 4] = 0.0f; verticesLinterna[13*6 + 5] = 0.0f;
                        verticesLinterna[14*6 + 3] = 0.0f; verticesLinterna[14*6 + 4] = 0.0f; verticesLinterna[14*6 + 5] = 0.0f;
                        verticesLinterna[15*6 + 3] = 0.0f; verticesLinterna[15*6 + 4] = 0.0f; verticesLinterna[15*6 + 5] = 0.0f;
                    }
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesLinterna), verticesLinterna);
                }
            }
            auto loadingElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - loadingStartTime);
            if (loadingElapsed.count() >= 8) {
                currentState = PLAY;
                inicializarBodega();
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                firstMouse = true;
                puntosIndex = 0;
                tiempoMensajeInicial = DURACION_MENSAJE;
                juegoPausado = false;
            }
        }

        // Actualizar escenario 3D (solo en PLAY y no en pausa)
        if (currentState == PLAY && juego3DInicializado && escenario && !juegoPausado && !menuMapa->isVisible()) {
            escenario->update(deltaTime);
            processInput(window, deltaTime);

            if (!eventoSalidaActivado && jugadorEnZonaSalida(cameraPos)) {
                eventoSalidaActivado = true;
                puertaBloqueada = true;
                if (escenario->isPuertaAbierta()) {
                    escenario->togglePuerta();
                }
                std::cout << "\n=== EVENTO ACTIVADO: puerta bloqueada ===" << std::endl;
            }

            // Detectar recolección del mapa
            if (!mapaObtenido) {
                glm::vec3 posicionMapa = glm::vec3(-3.0f, -1.0f, 14.7f);
                float distanciaAlMapa = glm::distance(cameraPos, posicionMapa);
                if (distanciaAlMapa < 3.0f) {
                    static bool eMapaPresionada = false;
                    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !eMapaPresionada) {
                        mapaObtenido = true;
                        menuMapa->enable();
                        tiempoMensajeMapaObtenido = DURACION_MENSAJE;
                        if (tiempoMensajeInicial > 0) tiempoMensajeInicial = 0.0f;
                        std::cout << "\n=== ¡HAS OBTENIDO EL MAPA! ===" << std::endl;
                        eMapaPresionada = true;
                    }
                    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
                        eMapaPresionada = false;
                    }
                }
            }
        } else if (currentState != PLAY && !juego3DInicializado) {
            processInput(window, deltaTime);
        } else if (currentState == PLAY && juego3DInicializado && (juegoPausado || menuMapa->isVisible())) {
            // En pausa o mapa visible, no actualizar escenario pero sí procesar input básico
            processInput(window, deltaTime);
        } else {
            processInput(window, deltaTime);
        }

        // Limpiar pantalla
        if (currentState == LOADING) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        } else if ((currentState == PLAY || currentState == PAUSE_MENU || currentState == MANUAL_FROM_PAUSE) && juego3DInicializado) {
            glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
        } else {
            glClearColor(0.04f, 0.08f, 0.17f, 1.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========== RENDERIZADO 3D ==========
        // Dibujar el escenario 3D en TODOS los estados donde el juego está inicializado
        if (juego3DInicializado && escenario && skybox) {
            // Asegurar estado OpenGL correcto para 3D
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            glm::mat4 projection = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
            skybox->render(view, projection);

            // Congelar animaciones en menús (tiempo = 0) para que no se vean movimientos extraños
            float tiempoAnimacion = (currentState == PLAY && !juegoPausado && !menuMapa->isVisible()) ? (float)glfwGetTime() : 0.0f;
            escenario->render(view, projection, cameraPos, tiempoAnimacion);
        }

        // ========== RENDERIZADO 2D (menús geométricos) ==========
        // Solo se dibujan en LOADING y MENU_PRINCIPAL/MANUAL/CREDITS
        if (currentState == LOADING || currentState == MENU_PRINCIPAL || currentState == MANUAL || currentState == CREDITS) {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            shaderProgram.use();

            if (currentState == LOADING) {
                glUniform1f(uniID, 2.2f);
                glBindVertexArray(VAO_Linterna);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            else if (currentState == MENU_PRINCIPAL || currentState == MANUAL || currentState == CREDITS) {
                glUniform1f(uniID, 1.0f);
                glBindVertexArray(VAO_PanelIzq); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_PanelDer); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_LineaBlanca); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_LineaMorada); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_AdornoSuperior); glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_MenuFondo); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_MarcoSuperior); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_DecoracionEsquina); glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_PanelInferior); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_PanelOscuro); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_DetalleAzul); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_BotonIzquierdo); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_BotonCentro); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_BotonDerecho); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(VAO_LuzCentral); glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }

        // ========== IMGUI - MENÚS ==========
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float escalaImGui = std::min(1.0f, std::max(0.8f, (float)windowWidth / 1280.0f));
        ImGui::GetIO().FontGlobalScale = 1.6f * escalaImGui;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20 * escalaImGui, 15 * escalaImGui));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10 * escalaImGui, 15 * escalaImGui));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(25 * escalaImGui, 25 * escalaImGui));
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 0.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.15f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.25f, 0.55f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.02f, 0.1f, 0.25f, 1.0f));

        // ========== MENÚ PRINCIPAL ==========
        if (currentState == MENU_PRINCIPAL) {
            float menuAncho = 500 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2(50, windowHeight / 2 - 30 * escalaImGui), ImGuiCond_Always, ImVec2(0.0f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(menuAncho, 700 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::Begin("Menu Principal", NULL,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);

            float anchoTextura = 430 * escalaImGui;
            float altoTextura = 190 * escalaImGui;
            ImGui::SetCursorPosX((menuAncho - anchoTextura) / 2);
            if (texturaCargada && texturaTituloID != 0) {
                ImGui::Image((void*)(intptr_t)texturaTituloID, ImVec2(anchoTextura, altoTextura));
            } else {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 p_min = ImGui::GetCursorScreenPos();
                ImVec2 p_max = ImVec2(p_min.x + anchoTextura, p_min.y + altoTextura);
                draw_list->AddRectFilled(p_min, p_max, IM_COL32(255, 255, 255, 255));
                ImGui::Dummy(ImVec2(anchoTextura, altoTextura));
                ImVec2 textSize = ImGui::CalcTextSize("UNSTABLE ANCHOR");
                ImGui::SetCursorPosX((menuAncho - textSize.x) / 2);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 85 * escalaImGui);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                ImGui::Text("UNSTABLE ANCHOR");
                ImGui::PopStyleColor();
            }

            ImGui::Spacing();
            ImGui::Spacing();

            float botonAncho = 410 * escalaImGui;
            float botonAlto = 75 * escalaImGui;

            if (!manualVisto) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
                ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
                if (ImGui::Button("PLAY", ImVec2(botonAncho, botonAlto))) {
                    tiempoMensajeBloqueo = DURACION_MENSAJE_BLOQUEO;
                }
                ImGui::PopStyleColor();
            } else {
                ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
                if (ImGui::Button("PLAY", ImVec2(botonAncho, botonAlto))) {
                    currentState = LOADING;
                    loadingStartTime = std::chrono::steady_clock::now();
                    puntosIndex = 0;
                    luzBlancoAmarillentoEncendida = true;
                    if (vbLinternaPtr) {
                        glBindBuffer(GL_ARRAY_BUFFER, *vbLinternaPtr);
                        verticesLinterna[12*6 + 3] = 1.0f; verticesLinterna[12*6 + 4] = 0.95f; verticesLinterna[12*6 + 5] = 0.7f;
                        verticesLinterna[13*6 + 3] = 1.0f; verticesLinterna[13*6 + 4] = 0.95f; verticesLinterna[13*6 + 5] = 0.7f;
                        verticesLinterna[14*6 + 3] = 1.0f; verticesLinterna[14*6 + 4] = 0.95f; verticesLinterna[14*6 + 5] = 0.7f;
                        verticesLinterna[15*6 + 3] = 1.0f; verticesLinterna[15*6 + 4] = 0.95f; verticesLinterna[15*6 + 5] = 0.7f;
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesLinterna), verticesLinterna);
                    }
                }
            }

            ImGui::Spacing();
            ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
            if (ImGui::Button("MANUAL", ImVec2(botonAncho, botonAlto))) {
                currentState = MANUAL;
            }

            ImGui::Spacing();
            ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
            if (ImGui::Button("CREDITS", ImVec2(botonAncho, botonAlto))) {
                currentState = CREDITS;
            }

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.25f, 0.25f, 1.0f));
            ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
            if (ImGui::Button("QUIT", ImVec2(botonAncho, botonAlto))) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::PopStyleColor(2);
            ImGui::End();

            if (tiempoMensajeBloqueo > 0.0f) {
                ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                ImGui::SetNextWindowSize(ImVec2(500 * escalaImGui, 140 * escalaImGui));
                ImGui::SetNextWindowBgAlpha(0.95f);
                ImGui::Begin("Mensaje", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
                ImGui::SetCursorPosY(35 * escalaImGui);
                ImGui::SetCursorPosX((500 * escalaImGui - ImGui::CalcTextSize("Para desbloquear PLAY,").x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
                ImGui::Text("Para desbloquear PLAY,");
                ImGui::SetCursorPosX((500 * escalaImGui - ImGui::CalcTextSize("vea el MANUAL primero").x) * 0.5f);
                ImGui::SetCursorPosY(75 * escalaImGui);
                ImGui::Text("vea el MANUAL primero");
                ImGui::PopStyleColor();
                ImGui::End();
            }
        }
        // ========== PANTALLA DE CARGA ==========
        else if (currentState == LOADING) {
            ImGui::SetNextWindowPos(ImVec2(0, windowHeight - 150));
            ImGui::SetNextWindowSize(ImVec2(windowWidth, 100));
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::Begin("Texto Carga", NULL,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
            std::string textoBase = "Cargando";
            std::string puntos = "";
            for (int i = 0; i < puntosIndex; i++) puntos += ".";
            std::string textoCompleto = textoBase + puntos;
            ImVec2 textSize = ImGui::CalcTextSize(textoCompleto.c_str());
            ImGui::SetCursorPos(ImVec2((windowWidth / 2) - (textSize.x / 2), 20));
            ImGui::SetWindowFontScale(3.0f * escalaImGui);
            ImGui::Text("%s", textoCompleto.c_str());
            ImGui::End();
        }
        // ========== MANUAL ==========
        else if (currentState == MANUAL) {
            float manualAncho = 950 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(manualAncho, 880 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.92f);
            ImGui::Begin("Manual de Controles", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("MANUAL DE CONTROLES                ").x) / 2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.0f * escalaImGui);
            ImGui::Text("MANUAL DE CONTROLES");
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.1f * escalaImGui);
            ImGui::Separator();
            ImGui::Spacing();

            // MOVIMIENTO
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== MOVIMIENTO ===").x) / 2);
            ImGui::Text("=== MOVIMIENTO ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            float centerX = manualAncho / 2;

            ImGui::SetCursorPosX(centerX - 45 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("W", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Caminar hacia adelante");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("A", ImVec2(80 * escalaImGui, 50 * escalaImGui))) {}
            ImGui::SameLine();
            if (ImGui::Button("S", ImVec2(80 * escalaImGui, 50 * escalaImGui))) {}
            ImGui::SameLine();
            if (ImGui::Button("D", ImVec2(80 * escalaImGui, 50 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 100 * escalaImGui);
            ImGui::Text("  Movimiento lateral");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("SHIFT", ImVec2(120 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Correr (mantener presionado)");

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // INTERACCIÓN
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.9f, 0.5f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== INTERACCION ===").x) / 2);
            ImGui::Text("=== INTERACCION ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("E", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Interactuar con objetos");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("F", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Encender/Apagar linterna");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("T", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Pausar/Reanudar musica");

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // CONTROLES DE MAPA
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 1.0f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== CONTROLES DE MAPA ===").x) / 2);
            ImGui::Text("=== CONTROLES DE MAPA ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("M", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Abrir mapa (despues de obtenerlo)");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("Z", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Cerrar mapa");

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // CONTROLES GENERALES
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.2f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== CONTROLES GENERALES ===").x) / 2);
            ImGui::Text("=== CONTROLES GENERALES ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("ESC", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Abrir menu de pausa");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button(" G ", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Pantalla completa / Ventana");

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SetCursorPosX((manualAncho - 340 * escalaImGui) / 2);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            if (ImGui::Button("VOLVER AL MENU PRINCIPAL", ImVec2(340 * escalaImGui, 60 * escalaImGui))) {
                manualVisto = true;
                currentState = MENU_PRINCIPAL;
            }
            ImGui::PopStyleColor();
            ImGui::End();
        }
        // ========== CRÉDITOS ==========
        else if (currentState == CREDITS) {
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(450 * escalaImGui, 400 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("Creditos", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetCursorPosX((450 * escalaImGui - ImGui::CalcTextSize("CREDITOS").x) / 2);
            ImGui::Text("CREDITOS");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SetCursorPosX(40 * escalaImGui);
            ImGui::TextWrapped("BODEGA DEL TERROR");
            ImGui::TextWrapped("Version 1.0");
            ImGui::Spacing();
            ImGui::TextWrapped("Desarrollado por:");
            ImGui::TextWrapped("Tu Nombre");
            ImGui::Spacing();
            ImGui::TextWrapped("Gracias por jugar");
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            if (ImGui::Button("Volver", ImVec2(140 * escalaImGui, 50 * escalaImGui))) {
                currentState = MENU_PRINCIPAL;
            }
            ImGui::PopStyleColor();
            ImGui::End();
        }
        // ========== MENÚ DE PAUSA ==========
        else if (currentState == PAUSE_MENU) {
            float pauseAncho = 500 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(pauseAncho, 450 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("Menu de Pausa", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            ImGui::SetCursorPosX((pauseAncho - ImGui::CalcTextSize("MENU DE PAUSA           ").x) / 2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.0f * escalaImGui);
            ImGui::Text("MENU DE PAUSA");
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.1f * escalaImGui);
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();

            float botonAncho = 410 * escalaImGui;
            float botonAlto = 70 * escalaImGui;

            ImGui::SetCursorPosX((pauseAncho - botonAncho) / 2);
            if (ImGui::Button("REANUDAR JUEGO", ImVec2(botonAncho, botonAlto))) {
                juegoPausado = false;
                currentState = PLAY;
                if (cameraStateSaved) {
                    cameraFront = savedCameraFront;
                    yaw = savedYaw;
                    pitch = savedPitch;
                    cameraStateSaved = false;
                }
                glfwSetCursorPos(window, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f);
                lastX = SCR_WIDTH / 2.0f;
                lastY = SCR_HEIGHT / 2.0f;
                firstMouse = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                std::cout << "Juego reanudado desde menú" << std::endl;
            }

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX((pauseAncho - botonAncho) / 2);
            if (ImGui::Button("MANUAL", ImVec2(botonAncho, botonAlto))) {
                currentState = MANUAL_FROM_PAUSE;
                std::cout << "Abriendo manual desde pausa" << std::endl;
            }

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX((pauseAncho - botonAncho) / 2);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.25f, 0.25f, 1.0f));
            if (ImGui::Button("VOLVER AL MENU PRINCIPAL", ImVec2(botonAncho, botonAlto))) {
                reiniciarJuego();
                juegoPausado = false;
                currentState = MENU_PRINCIPAL;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                std::cout << "Volviendo al menú principal" << std::endl;
            }
            ImGui::PopStyleColor(2);

            ImGui::End();
        }
        // ========== MANUAL DESDE PAUSA ==========
        else if (currentState == MANUAL_FROM_PAUSE) {
            float manualAncho = 950 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(manualAncho, 880 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("Manual de Controles", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("MANUAL DE CONTROLES                ").x) / 2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.0f * escalaImGui);
            ImGui::Text("MANUAL DE CONTROLES");
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.1f * escalaImGui);
            ImGui::Separator();
            ImGui::Spacing();

            // MOVIMIENTO
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== MOVIMIENTO ===").x) / 2);
            ImGui::Text("=== MOVIMIENTO ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            float centerX = manualAncho / 2;

            ImGui::SetCursorPosX(centerX - 45 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("W", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Caminar hacia adelante");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("A", ImVec2(80 * escalaImGui, 50 * escalaImGui))) {}
            ImGui::SameLine();
            if (ImGui::Button("S", ImVec2(80 * escalaImGui, 50 * escalaImGui))) {}
            ImGui::SameLine();
            if (ImGui::Button("D", ImVec2(80 * escalaImGui, 50 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 100 * escalaImGui);
            ImGui::Text("  Movimiento lateral");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("SHIFT", ImVec2(120 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Correr (mantener presionado)");

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // INTERACCIÓN
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.9f, 0.5f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== INTERACCION ===").x) / 2);
            ImGui::Text("=== INTERACCION ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("E", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Interactuar con objetos");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("F", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Encender/Apagar linterna");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("T", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Pausar/Reanudar musica");

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // CONTROLES DE MAPA
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 1.0f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== CONTROLES DE MAPA ===").x) / 2);
            ImGui::Text("=== CONTROLES DE MAPA ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("M", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Abrir mapa (despues de obtenerlo)");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("Z", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Cerrar mapa");

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // CONTROLES GENERALES
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.2f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== CONTROLES GENERALES ===").x) / 2);
            ImGui::Text("=== CONTROLES GENERALES ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("ESC", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Abrir menu de pausa");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button(" G ", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Pantalla completa / Ventana");

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SetCursorPosX((manualAncho - 340 * escalaImGui) / 2);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            if (ImGui::Button("VOLVER AL MENU DE PAUSA", ImVec2(340 * escalaImGui, 60 * escalaImGui))) {
                currentState = PAUSE_MENU;
                std::cout << "Volviendo al menú de pausa" << std::endl;
            }
            ImGui::PopStyleColor();
            ImGui::End();
        }

        // ========== MENSAJES DE LA BODEGA ==========
        if (currentState == PLAY && !mapaObtenido && tiempoMensajeInicial > 0.0f && !juegoPausado) {
            float windowWidthMsg = 1100 * escalaImGui;
            float windowHeightMsg = 350 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2((windowWidth - windowWidthMsg) * 0.5f, (windowHeight - windowHeightMsg) * 0.3f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(windowWidthMsg, windowHeightMsg));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("MensajeInicial", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Dummy(ImVec2(0.0f, 30 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("INTERACTUA CON EL MAPA            ").x) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.2f * escalaImGui);
            ImGui::Text("INTERACTUA CON EL MAPA");

            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("Presiona E sobre el cuadro blanco con ").x) * 0.5f);
            ImGui::SetWindowFontScale(1.8f * escalaImGui);
            ImGui::Text("Presiona E sobre el cuadro blanco con el mapa");

            ImGui::Dummy(ImVec2(0.0f, 15 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("Busca en la bodega princi").x) * 0.5f);
            ImGui::SetWindowFontScale(1.5f * escalaImGui);
            ImGui::Text("Busca en la bodega principal");
            ImGui::PopStyleColor();
            ImGui::End();
        }

        if (currentState == PLAY && mapaObtenido && tiempoMensajeMapaObtenido > 0.0f && !juegoPausado) {
            float windowWidthMsg = 900 * escalaImGui;
            float windowHeightMsg = 250 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2((windowWidth - windowWidthMsg) * 0.5f, (windowHeight - windowHeightMsg) * 0.3f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(windowWidthMsg, windowHeightMsg));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("MensajeMapa", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Dummy(ImVec2(0.0f, 20 * escalaImGui));

            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("¡HAS OBTENIDO EL MAPA!   ").x) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.9f, 0.3f, 1.0f));
            ImGui::SetWindowFontScale(1.8f * escalaImGui);
            ImGui::Text("¡HAS OBTENIDO EL MAPA!");

            ImGui::Dummy(ImVec2(0.0f, 10 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("Presiona M para abrir el ").x) * 0.5f);
            ImGui::SetWindowFontScale(1.5f * escalaImGui);
            ImGui::Text("Presiona M para abrir el mapa");

            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("Presiona Z para cerrar el mapa").x) * 0.5f);
            ImGui::SetWindowFontScale(1.5f * escalaImGui);
            ImGui::Text("Presiona Z para cerrar el mapa");

            ImGui::PopStyleColor();
            ImGui::End();
        }

        // Indicador de mapa disponible
        if (currentState == PLAY && mapaObtenido && !juegoPausado && tiempoMensajeMapaObtenido <= 0.0f) {
            ImGui::SetNextWindowPos(ImVec2(windowWidth - 400 * escalaImGui, 20 * escalaImGui));
            ImGui::SetNextWindowSize(ImVec2(380 * escalaImGui, 120 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.8f);
            ImGui::Begin("InfoMapa", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowFontScale(1.2f * escalaImGui);
            ImGui::Text("M para abrir mapa");
            ImGui::Text("Z para cerrar mapa");
            ImGui::End();
        }

        // ========== RENDERIZAR MENÚ DEL MAPA ==========
        if (currentState == PLAY && menuMapa && menuMapa->isEnabled() && menuMapa->isVisible()) {
            float windowWidthMap = 750 * escalaImGui;
            float windowHeightMap = 750 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2((windowWidth - windowWidthMap) * 0.5f, (windowHeight - windowHeightMap) * 0.35f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(windowWidthMap, windowHeightMap));
            ImGui::SetNextWindowBgAlpha(0.9f);
            ImGui::Begin("MenuMapa", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Dummy(ImVec2(0.0f, 20 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMap - ImGui::CalcTextSize("MAPA DE LA BODEGA      ").x) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.5f * escalaImGui);
            ImGui::Text("MAPA DE LA BODEGA");
            ImGui::PopStyleColor();
            ImGui::Dummy(ImVec2(0.0f, 20 * escalaImGui));
            if (menuMapa->getTextureID() != 0) {
                float imageSize = 600 * escalaImGui;
                ImGui::SetCursorPosX((windowWidthMap - imageSize) * 0.5f);
                ImGui::Image((void*)(intptr_t)menuMapa->getTextureID(), ImVec2(imageSize, imageSize));
            }
            ImGui::Dummy(ImVec2(0.0f, 20 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMap - ImGui::CalcTextSize("Presiona Z para cerrrr").x) * 0.5f);
            ImGui::SetWindowFontScale(1.85f * escalaImGui);
            ImGui::Text("Presiona Z para cerrar el mapa");
            ImGui::End();
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(5);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ========== LIMPIEZA ==========
    if (juego3DInicializado) {
        delete escenario;
        delete skybox;
        escenario = nullptr;
        skybox = nullptr;
    }
    musicaFondo.stop();
    delete menuMapa;
    glDeleteTextures(1, &texturaTituloID);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}