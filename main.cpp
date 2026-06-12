#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

// Audio SFML
#include <SFML/Audio.hpp>

// ImGui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Skybox.h"
#include "Escenario.h"
#include "Shader.h"
#include "Menu.h"
#include "Menu2D.h"

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Variables de la bodega 3D
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
bool mapaObtenido = false;

// Variables para mensajes en pantalla
float tiempoMensajeInicial = 0.0f;
float tiempoMensajeMapaObtenido = 0.0f;
const float DURACION_MENSAJE = 4.0f;

// Estado de salida
bool eventoSalidaActivado = false;
bool puertaBloqueada = false;

bool verificarColisionParedes(glm::vec3 nuevaPos) {
    // ==========================================
    // NUEVO PASILLO RECEPCIÓN
    // ==========================================
    if (nuevaPos.z > 74.0f && nuevaPos.z <= 114.0f) {
        float xMin = 13.5f + RADIO_JUGADOR;
        float xMax = 23.5f - RADIO_JUGADOR;

        // Paredes laterales: si está fuera del ancho, es colisión (retorna false)
        if (nuevaPos.x < xMin || nuevaPos.x > xMax) {
            // Verificar si está en alguna de las 3 puertas laterales
            bool enPuertaLateral = false;
            for(int i = 0; i < 3; i++) {
                float zPuerta = 79.0f + (i * 10.0f);
                if (nuevaPos.z > zPuerta - 1.1f && nuevaPos.z < zPuerta + 1.1f) enPuertaLateral = true;
            }
            if (!enPuertaLateral) return false;
        }

        // Puerta de cristal al final (Z = 114.0)
        bool enPuertaCristal = (nuevaPos.x > 17.0f && nuevaPos.x < 20.0f);
        if (nuevaPos.z > 114.0f - RADIO_JUGADOR && !enPuertaCristal) return false;

        return true;
    }

    // 1. SALA DE RECEPCIÓN (Última sala)
    if (nuevaPos.z > 56.0f && nuevaPos.z <= 74.0f && nuevaPos.x > 9.0f) {
        float xMin = 9.5f + RADIO_JUGADOR;
        float xMax = 27.5f - RADIO_JUGADOR;
        float zMin = 56.0f + RADIO_JUGADOR;
        float zMax = 74.0f - RADIO_JUGADOR;

        if (nuevaPos.x < xMin || nuevaPos.x > xMax) return false;
        bool enPuertaFrente = (nuevaPos.x > 17.4f && nuevaPos.x < 19.6f);
        if (nuevaPos.z > zMax && !enPuertaFrente) return false;
        bool enPuertaAtras = (nuevaPos.x > 17.0f && nuevaPos.x < 20.0f);
        if (nuevaPos.z < zMin && !enPuertaAtras) return false;
        return true;
    }

    if (nuevaPos.x > 27.5f) {
        float xMax = 37.5f - RADIO_JUGADOR;
        float zMin = 35.5f + RADIO_JUGADOR;
        float zMax = 38.5f - RADIO_JUGADOR;
        if (nuevaPos.x > xMax) return false;
        if (nuevaPos.z < zMin) return false;
        if (nuevaPos.z > zMax) return false;
        return true;
    }

    // 2. PASILLO ANTES DE LA RECEPCIÓN
    if (nuevaPos.z > 46.0f && nuevaPos.x > 9.5f && nuevaPos.z <= 56.0f) {
        float zMax = 56.0f - RADIO_JUGADOR;
        float xMin = 17.0f + RADIO_JUGADOR;
        float xMax = 20.0f - RADIO_JUGADOR;
        if (nuevaPos.x < xMin || nuevaPos.x > xMax) return false;
        bool enPuertaFondo = (nuevaPos.x > 17.0f && nuevaPos.x < 20.0f);
        if (nuevaPos.z > zMax && !enPuertaFondo) return false;
        return true;
    }

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

    if (nuevaPos.z > 44.0f && nuevaPos.z <= 54.0f && nuevaPos.x <= 9.5f) {
        if (nuevaPos.x < -9.0f + RADIO_JUGADOR || nuevaPos.x > -6.0f - RADIO_JUGADOR) return false;
        return true;
    }

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

    if (nuevaPos.z > 15.0f && nuevaPos.z <= 30.0f) {
        if (nuevaPos.x < -8.8f || nuevaPos.x > -6.2f) return false;
        if (nuevaPos.z > 29.5f && (nuevaPos.x < -8.5f || nuevaPos.x > -6.5f)) return false;
        return true;
    }

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
        if (pasoActual == 1) { soundPaso.setBuffer(bufferPaso1); pasoActual = 2; }
        else if (pasoActual == 2) { soundPaso.setBuffer(bufferPaso2); pasoActual = 3; }
        else { soundPaso.setBuffer(bufferPaso3); pasoActual = 1; }
        soundPaso.setVolume(55.0f);
    } else {
        if (pasoActual == 1) { soundPaso.setBuffer(bufferPaso1); pasoActual = 2; }
        else if (pasoActual == 2) { soundPaso.setBuffer(bufferPaso2); pasoActual = 3; }
        else { soundPaso.setBuffer(bufferPaso3); pasoActual = 1; }
        soundPaso.setVolume(40.0f);
    }
    soundPaso.play();
}

bool jugadorEnZonaSalida(glm::vec3 pos) {
    return (pos.x > -9.0f && pos.x < -6.0f && pos.z > 28.0f && pos.z < 31.5f);
}

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

void processInput(GLFWwindow* window, float deltaTime) {
    // ===== TECLA G: PANTALLA COMPLETA (siempre funciona) =====
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

    // ===== TECLA Z: CERRAR MAPA =====
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
    }
    lastZState = currentZState;

    if (juegoPausado || (menuMapa && menuMapa->isVisible())) return;

    if (currentState == PLAY) {
        float cameraSpeed = 2.5f * deltaTime;
        bool corriendo = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        if (corriendo) cameraSpeed = 5.0f * deltaTime;

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
            estaCorriendo = corriendo;
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
        if (currentFState && !lastFState) flashlightOn = !flashlightOn;
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
            }
            ePresionada = true;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) ePresionada = false;

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
        }
        lastMState = currentMState;

        // Tecla T: pausar música
        static bool lastTState = false;
        bool currentTState = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
        if (currentTState && !lastTState) {
            if (musicaReproduciendo) { musicaFondo.pause(); musicaReproduciendo = false; }
            else { musicaFondo.play(); musicaReproduciendo = true; }
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
        }
    }
    lastEscState = currentEscState;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (juegoPausado || currentState != PLAY) return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;
    float sensitivity = 0.1f;
    xoffset *= sensitivity; yoffset *= sensitivity;
    yaw += xoffset; pitch += yoffset;
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

void inicializarBodega() {
    if (juego3DInicializado) return;
    std::cout << "=== INICIALIZANDO BODEGA 3D ===" << std::endl;
    if (musicaFondo.openFromFile("sounds/Ambience/Ambient_sound.ogg")) {
        musicaFondo.setLoop(true);
        musicaFondo.setVolume(50.0f);
        musicaFondo.play();
        musicaReproduciendo = true;
    }
    if (bufferPaso1.loadFromFile("sounds/SFX/Pasos_1.wav")) std::cout << "Paso 1 cargado" << std::endl;
    if (bufferPaso2.loadFromFile("sounds/SFX/Pasos_2.wav")) std::cout << "Paso 2 cargado" << std::endl;
    if (bufferPaso3.loadFromFile("sounds/SFX/Pasos_3.wav")) std::cout << "Paso 3 cargado" << std::endl;
    if (bufferPuerta.loadFromFile("sounds/SFX/puerta.wav")) {
        soundPuerta.setBuffer(bufferPuerta);
        sonidoCargado = true;
        soundPuerta.setVolume(60.0f);
    }
    if (bufferPalanca.loadFromFile("sounds/SFX/palanca.wav")) {
        soundPalanca.setBuffer(bufferPalanca);
        sonidoPalancaCargado = true;
        soundPalanca.setVolume(60.0f);
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
    yaw = -90.0f; pitch = 0.0f;
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
    if (menuMapa) { menuMapa->disable(); menuMapa->setVisible(false); }
    tiempoMensajeInicial = DURACION_MENSAJE;
    tiempoMensajeMapaObtenido = 0.0f;
}

int main() {
    std::cout << "=== INICIANDO KENNY'S WAREHOUSE ===" << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "UNSTABLE ANOMALY", NULL, NULL);
    if (!window) { std::cout << "ERROR: No se pudo crear la ventana" << std::endl; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "ERROR: No se pudo inicializar GLAD" << std::endl; return -1; }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.6f;

    // Cargar textura del título
    unsigned int texturaTituloID = cargarTexturaMenu("Textures/Unstable.png");
    bool texturaCargada = (texturaTituloID != 0);

    // Shader 2D
    Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    // Inicializar menú 2D
    inicializarMenu2D(shaderProgram);

    // Inicializar menú del mapa
    menuMapa = new Menu();
    if (!menuMapa->loadTexture("Textures/mapa.png")) std::cout << "Advertencia: No se pudo cargar la textura del mapa" << std::endl;
    menuMapa->disable();
    menuMapa->setVisible(false);

    lastFrame = 0.0f;
    tiempoMensajeInicial = DURACION_MENSAJE;

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
            if (tiempoMensajeInicial > 0.0f) { tiempoMensajeInicial -= deltaTime; if (tiempoMensajeInicial < 0.0f) tiempoMensajeInicial = 0.0f; }
            if (tiempoMensajeMapaObtenido > 0.0f) { tiempoMensajeMapaObtenido -= deltaTime; if (tiempoMensajeMapaObtenido < 0.0f) tiempoMensajeMapaObtenido = 0.0f; }
        }

        if (currentState != PLAY && currentState != LOADING && currentState != PAUSE_MENU && currentState != MANUAL_FROM_PAUSE) {
            actualizarParpadeoLuzCentral();
        }

        if (currentState == LOADING) {
            auto puntoElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPuntoTime);
            if (puntoElapsed.count() >= 500) { puntosIndex = (puntosIndex + 1) % 4; lastPuntoTime = now; }
            auto luzBlinkElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLuzBlink);
            if (luzBlinkElapsed.count() >= 300) { luzBlancoAmarillentoEncendida = !luzBlancoAmarillentoEncendida; lastLuzBlink = now; actualizarParpadeoLinternaCarga(); }
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

        if (currentState == PLAY && juego3DInicializado && escenario && !juegoPausado && !menuMapa->isVisible()) {
            escenario->update(deltaTime);
            processInput(window, deltaTime);
            if (!eventoSalidaActivado && jugadorEnZonaSalida(cameraPos)) {
                eventoSalidaActivado = true;
                puertaBloqueada = true;
                if (escenario->isPuertaAbierta()) escenario->togglePuerta();
            }
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
                        eMapaPresionada = true;
                    }
                    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) eMapaPresionada = false;
                }
            }
        } else if (currentState != PLAY && !juego3DInicializado) {
            processInput(window, deltaTime);
        } else if (currentState == PLAY && juego3DInicializado && (juegoPausado || menuMapa->isVisible())) {
            processInput(window, deltaTime);
        } else {
            processInput(window, deltaTime);
        }

        if (currentState == LOADING) glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        else if ((currentState == PLAY || currentState == PAUSE_MENU || currentState == MANUAL_FROM_PAUSE) && juego3DInicializado) glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
        else glClearColor(0.04f, 0.08f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (juego3DInicializado && escenario && skybox) {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            glm::mat4 projection = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
            skybox->render(view, projection);
            float tiempoAnimacion = (currentState == PLAY && !juegoPausado && !menuMapa->isVisible()) ? (float)glfwGetTime() : 0.0f;
            escenario->render(view, projection, cameraPos, tiempoAnimacion);
        }

        if (currentState == LOADING || currentState == MENU_PRINCIPAL || currentState == MANUAL || currentState == CREDITS) {
            dibujarMenu2D(shaderProgram, uniID, windowWidth, windowHeight, std::min(1.0f, std::max(0.8f, (float)windowWidth / 1280.0f)));
        }

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

        // MENÚ PRINCIPAL
        if (currentState == MENU_PRINCIPAL) {
            float menuAncho = 500 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2(50, windowHeight / 2 - 30 * escalaImGui), ImGuiCond_Always, ImVec2(0.0f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(menuAncho, 700 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::Begin("Main Menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
            float anchoTextura = 430 * escalaImGui;
            float altoTextura = 190 * escalaImGui;
            ImGui::SetCursorPosX((menuAncho - anchoTextura) / 2);
            if (texturaCargada && texturaTituloID != 0) ImGui::Image((void*)(intptr_t)texturaTituloID, ImVec2(anchoTextura, altoTextura));
            else { ImGui::Dummy(ImVec2(anchoTextura, altoTextura)); ImVec2 textSize = ImGui::CalcTextSize("UNSTABLE ANCHOR"); ImGui::SetCursorPosX((menuAncho - textSize.x) / 2); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 85 * escalaImGui); ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); ImGui::Text("UNSTABLE ANCHOR"); ImGui::PopStyleColor(); }
            ImGui::Spacing(); ImGui::Spacing();
            float botonAncho = 410 * escalaImGui;
            float botonAlto = 75 * escalaImGui;
            if (!manualVisto) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
                ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
                if (ImGui::Button("PLAY", ImVec2(botonAncho, botonAlto))) tiempoMensajeBloqueo = DURACION_MENSAJE_BLOQUEO;
                ImGui::PopStyleColor();
            } else {
                ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
                if (ImGui::Button("PLAY", ImVec2(botonAncho, botonAlto))) { currentState = LOADING; loadingStartTime = std::chrono::steady_clock::now(); puntosIndex = 0; luzBlancoAmarillentoEncendida = true; resetearColoresLinterna(); }
            }
            ImGui::Spacing();
            ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
            if (ImGui::Button("MANUAL", ImVec2(botonAncho, botonAlto))) currentState = MANUAL;
            ImGui::Spacing();
            ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
            if (ImGui::Button("CREDITS", ImVec2(botonAncho, botonAlto))) currentState = CREDITS;
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.25f, 0.25f, 1.0f));
            ImGui::SetCursorPosX((menuAncho - botonAncho) / 2);
            if (ImGui::Button("QUIT", ImVec2(botonAncho, botonAlto))) glfwSetWindowShouldClose(window, true);
            ImGui::PopStyleColor(2);
            ImGui::End();
            if (tiempoMensajeBloqueo > 0.0f) {
                ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                ImGui::SetNextWindowSize(ImVec2(500 * escalaImGui, 140 * escalaImGui));
                ImGui::SetNextWindowBgAlpha(0.95f);
                ImGui::Begin("Message", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
                ImGui::SetCursorPosY(35 * escalaImGui);
                ImGui::SetCursorPosX((500 * escalaImGui - ImGui::CalcTextSize("To unlock PLAY,").x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
                ImGui::Text("To unlock PLAY,");
                ImGui::SetCursorPosX((500 * escalaImGui - ImGui::CalcTextSize("Read the MANUAL first").x) * 0.5f);
                ImGui::SetCursorPosY(75 * escalaImGui);
                ImGui::Text("Read the MANUAL first");
                ImGui::PopStyleColor();
                ImGui::End();
            }
        }
        // PANTALLA DE CARGA
        else if (currentState == LOADING) {
            ImGui::SetNextWindowPos(ImVec2(0, windowHeight - 150));
            ImGui::SetNextWindowSize(ImVec2(windowWidth, 100));
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::Begin("Loading Text", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
            std::string textoBase = "Loading";
            std::string puntos = "";
            for (int i = 0; i < puntosIndex; i++) puntos += ".";
            std::string textoCompleto = textoBase + puntos;
            ImVec2 textSize = ImGui::CalcTextSize(textoCompleto.c_str());
            ImGui::SetCursorPos(ImVec2((windowWidth / 2) - (textSize.x / 2), 20));
            ImGui::SetWindowFontScale(3.0f * escalaImGui);
            ImGui::Text("%s", textoCompleto.c_str());
            ImGui::End();
        }
        // MANUAL
        else if (currentState == MANUAL) {
            float manualAncho = 950 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(manualAncho, 880 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.92f);
            ImGui::Begin("Controls Manual", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("Controls Manual               ").x) / 2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.0f * escalaImGui);
            ImGui::Text("Controls Manual");
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.1f * escalaImGui);
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== MOVEMENT ===").x) / 2);
            ImGui::Text("=== MOVEMENT ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            float centerX = manualAncho / 2;
            ImGui::SetCursorPosX(centerX - 45 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("W", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Move forward");
            ImGui::Spacing(); ImGui::Spacing();
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
            ImGui::Text("  Lateral movement");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("SHIFT", ImVec2(120 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Run (hold down)");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.9f, 0.5f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== INTERACTION ===").x) / 2);
            ImGui::Text("=== INTERACTION ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("E", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Interact with objects");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("F", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Turn flashlight on/off");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("T", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Pause/Resume music");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 1.0f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== MAP CONTROLS ===").x) / 2);
            ImGui::Text("=== MAP CONTROLS ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("M", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Open map (after obtaining it)");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("Z", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Close map");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.2f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== GENERAL CONTROLS ===").x) / 2);
            ImGui::Text("=== GENERAL CONTROLS ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("ESC", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Open pause menu");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button(" G ", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Fullscreen / Windowed");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SetCursorPosX((manualAncho - 340 * escalaImGui) / 2);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            if (ImGui::Button("BACK TO MAIN MENU", ImVec2(340 * escalaImGui, 60 * escalaImGui))) { manualVisto = true; currentState = MENU_PRINCIPAL; }
            ImGui::PopStyleColor();
            ImGui::End();
        }
        // CRÉDITOS
        else if (currentState == CREDITS) {
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(450 * escalaImGui, 400 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("Credits", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetCursorPosX((450 * escalaImGui - ImGui::CalcTextSize("CREDITS").x) / 2);
            ImGui::Text("CREDITS");
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
            if (ImGui::Button("Back", ImVec2(140 * escalaImGui, 50 * escalaImGui))) currentState = MENU_PRINCIPAL;
            ImGui::PopStyleColor();
            ImGui::End();
        }
        // MENÚ DE PAUSA
        else if (currentState == PAUSE_MENU) {
            float pauseAncho = 500 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(pauseAncho, 450 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("Pause Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetCursorPosX((pauseAncho - ImGui::CalcTextSize("PAUSE MENU           ").x) / 2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.0f * escalaImGui);
            ImGui::Text("PAUSE MENU");
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.1f * escalaImGui);
            ImGui::Separator();
            ImGui::Spacing(); ImGui::Spacing();
            float botonAncho = 410 * escalaImGui;
            float botonAlto = 70 * escalaImGui;
            ImGui::SetCursorPosX((pauseAncho - botonAncho) / 2);
            if (ImGui::Button("RESUME GAME", ImVec2(botonAncho, botonAlto))) { juegoPausado = false; currentState = PLAY; if (cameraStateSaved) { cameraFront = savedCameraFront; yaw = savedYaw; pitch = savedPitch; cameraStateSaved = false; } glfwSetCursorPos(window, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f); lastX = SCR_WIDTH / 2.0f; lastY = SCR_HEIGHT / 2.0f; firstMouse = true; glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX((pauseAncho - botonAncho) / 2);
            if (ImGui::Button("MANUAL", ImVec2(botonAncho, botonAlto))) currentState = MANUAL_FROM_PAUSE;
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX((pauseAncho - botonAncho) / 2);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.25f, 0.25f, 1.0f));
            if (ImGui::Button("BACK TO MAIN MENU", ImVec2(botonAncho, botonAlto))) { reiniciarJuego(); juegoPausado = false; currentState = MENU_PRINCIPAL; glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
            ImGui::PopStyleColor(2);
            ImGui::End();
        }
        // MANUAL DESDE PAUSA
        else if (currentState == MANUAL_FROM_PAUSE) {
            float manualAncho = 950 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(manualAncho, 880 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("Controls Manual", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("CONTROLS MANUAL                ").x) / 2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.0f * escalaImGui);
            ImGui::Text("CONTROLS MANUAL");
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.1f * escalaImGui);
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== MOVEMENT ===").x) / 2);
            ImGui::Text("=== MOVEMENT ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            float centerX = manualAncho / 2;
            ImGui::SetCursorPosX(centerX - 45 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("W", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Move forward");
            ImGui::Spacing(); ImGui::Spacing();
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
            ImGui::Text("  Lateral movement");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("SHIFT", ImVec2(120 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Run (hold down)");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.9f, 0.5f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== INTERACTION ===").x) / 2);
            ImGui::Text("=== INTERACTION ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("E", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Interact with objects");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("F", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Turn flashlight on/off");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("T", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Pause/Resume music");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 1.0f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== MAP CONTROLS ===").x) / 2);
            ImGui::Text("=== MAP CONTROLS ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("M", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Open map (after obtaining it)");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("Z", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Close map");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.2f, 1.0f));
            ImGui::SetCursorPosX((manualAncho - ImGui::CalcTextSize("=== GENERAL CONTROLS ===").x) / 2);
            ImGui::Text("=== GENERAL CONTROLS ===");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button("ESC", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Open pause menu");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centerX - 150 * escalaImGui);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
            if (ImGui::Button(" G ", ImVec2(90 * escalaImGui, 55 * escalaImGui))) {}
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::SetCursorPosX(centerX + 65 * escalaImGui);
            ImGui::Text("Fullscreen / Windowed");
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SetCursorPosX((manualAncho - 340 * escalaImGui) / 2);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            if (ImGui::Button("BACK TO PAUSE MENU", ImVec2(340 * escalaImGui, 60 * escalaImGui))) currentState = PAUSE_MENU;
            ImGui::PopStyleColor();
            ImGui::End();
        }

        // MENSAJES DE LA BODEGA
        if (currentState == PLAY && !mapaObtenido && tiempoMensajeInicial > 0.0f && !juegoPausado) {
            float windowWidthMsg = 1100 * escalaImGui;
            float windowHeightMsg = 350 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2((windowWidth - windowWidthMsg) * 0.5f, (windowHeight - windowHeightMsg) * 0.3f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(windowWidthMsg, windowHeightMsg));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("InitialMessage", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Dummy(ImVec2(0.0f, 30 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("INTERACT WITH THE MAP            ").x) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.2f * escalaImGui);
            ImGui::Text("INTERACT WITH THE MAP");
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("Press E on the white square with ").x) * 0.5f);
            ImGui::SetWindowFontScale(1.8f * escalaImGui);
            ImGui::Text("Press E on the white square with the map");
            ImGui::Dummy(ImVec2(0.0f, 15 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("Look in the main winery").x) * 0.5f);
            ImGui::SetWindowFontScale(1.5f * escalaImGui);
            ImGui::Text("Look in the main winery");
            ImGui::PopStyleColor();
            ImGui::End();
        }

        if (currentState == PLAY && mapaObtenido && tiempoMensajeMapaObtenido > 0.0f && !juegoPausado) {
            float windowWidthMsg = 900 * escalaImGui;
            float windowHeightMsg = 250 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2((windowWidth - windowWidthMsg) * 0.5f, (windowHeight - windowHeightMsg) * 0.3f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(windowWidthMsg, windowHeightMsg));
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Begin("MapMessage", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Dummy(ImVec2(0.0f, 20 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("YOU HAVE OBTAINED THE MAP!   ").x) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.9f, 0.3f, 1.0f));
            ImGui::SetWindowFontScale(1.8f * escalaImGui);
            ImGui::Text("YOU HAVE OBTAINED THE MAP!");
            ImGui::Dummy(ImVec2(0.0f, 10 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("Press M to open the ").x) * 0.5f);
            ImGui::SetWindowFontScale(1.5f * escalaImGui);
            ImGui::Text("Press M to open the map");
            ImGui::SetCursorPosX((windowWidthMsg - ImGui::CalcTextSize("Press Z to close the map").x) * 0.5f);
            ImGui::SetWindowFontScale(1.5f * escalaImGui);
            ImGui::Text("Press Z to close the map");
            ImGui::PopStyleColor();
            ImGui::End();
        }

        if (currentState == PLAY && mapaObtenido && !juegoPausado && tiempoMensajeMapaObtenido <= 0.0f) {
            ImGui::SetNextWindowPos(ImVec2(windowWidth - 400 * escalaImGui, 20 * escalaImGui));
            ImGui::SetNextWindowSize(ImVec2(380 * escalaImGui, 120 * escalaImGui));
            ImGui::SetNextWindowBgAlpha(0.8f);
            ImGui::Begin("MapInfo", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowFontScale(1.2f * escalaImGui);
            ImGui::Text("M to open map");
            ImGui::Text("Z to close map");
            ImGui::End();
        }

        if (currentState == PLAY && menuMapa && menuMapa->isEnabled() && menuMapa->isVisible()) {
            float windowWidthMap = 750 * escalaImGui;
            float windowHeightMap = 750 * escalaImGui;
            ImGui::SetNextWindowPos(ImVec2((windowWidth - windowWidthMap) * 0.5f, (windowHeight - windowHeightMap) * 0.35f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(windowWidthMap, windowHeightMap));
            ImGui::SetNextWindowBgAlpha(0.9f);
            ImGui::Begin("MapMenu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Dummy(ImVec2(0.0f, 20 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMap - ImGui::CalcTextSize("WINERY MAP      ").x) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::SetWindowFontScale(2.5f * escalaImGui);
            ImGui::Text("WINERY MAP");
            ImGui::PopStyleColor();
            ImGui::Dummy(ImVec2(0.0f, 20 * escalaImGui));
            if (menuMapa->getTextureID() != 0) {
                float imageSize = 600 * escalaImGui;
                ImGui::SetCursorPosX((windowWidthMap - imageSize) * 0.5f);
                ImGui::Image((void*)(intptr_t)menuMapa->getTextureID(), ImVec2(imageSize, imageSize));
            }
            ImGui::Dummy(ImVec2(0.0f, 20 * escalaImGui));
            ImGui::SetCursorPosX((windowWidthMap - ImGui::CalcTextSize("Press Z to close").x) * 0.5f);
            ImGui::SetWindowFontScale(1.85f * escalaImGui);
            ImGui::Text("Press Z to close the map");
            ImGui::End();
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(5);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (juego3DInicializado) { delete escenario; delete skybox; }
    musicaFondo.stop();
    delete menuMapa;
    glDeleteTextures(1, &texturaTituloID);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}