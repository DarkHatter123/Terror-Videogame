#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iomanip>

// Audio SFML
#include <SFML/Audio.hpp>

#include "Skybox.h"
#include "Escenario.h"
#include "Lampara.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Variables de cámara
glm::vec3 cameraPos = glm::vec3(0.0f, 1.5f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Límites de la habitación
const float HABITACION_ANCHO = 20.0f;
const float HABITACION_ALTO = 10.0f;
const float HABITACION_PROFUNDO = 30.0f;
const float MITAD_ANCHO = HABITACION_ANCHO / 2.0f;
const float MITAD_PROFUNDO = HABITACION_PROFUNDO / 2.0f;
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

// Variables para pasos (3 sonidos para mayor naturalidad)
sf::SoundBuffer bufferPaso1;
sf::SoundBuffer bufferPaso2;
sf::SoundBuffer bufferPaso3;
sf::Sound soundPaso;
int pasoActual = 1;
float intervaloPasos = 0.4f;

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

// ========================================
// REPRODUCIR SONIDO DE PASOS (alternando 1,2,3)
// ========================================
void reproducirPaso() {
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
    soundPaso.play();
}

// ========================================
// EVENTO DE SALIDA
// ========================================
bool eventoSalidaActivado = false;
bool puertaBloqueada = false;

bool jugadorEnZonaSalida(glm::vec3 pos) {
    if (pos.x > -9.0f && pos.x < -6.0f && pos.z > 28.0f && pos.z < 31.5f) {
        return true;
    }
    return false;
}

void processInput(GLFWwindow* window, Escenario& escenario, float deltaTime) {
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Control de música con tecla M
    static bool lastMState = false;
    bool currentMState = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
    if (currentMState && !lastMState) {
        if (musicaReproduciendo) {
            musicaFondo.pause();
            musicaReproduciendo = false;
            std::cout << "Música pausada" << std::endl;
        } else {
            musicaFondo.play();
            musicaReproduciendo = true;
            std::cout << "Música reanudada" << std::endl;
        }
    }
    lastMState = currentMState;

    glm::vec3 nuevaPos = cameraPos;
    bool mover = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 direccionHorizontal = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        nuevaPos += cameraSpeed * direccionHorizontal;
        mover = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 direccionHorizontal = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        nuevaPos -= cameraSpeed * direccionHorizontal;
        mover = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 derecha = glm::normalize(glm::cross(cameraFront, cameraUp));
        glm::vec3 derechaHorizontal = glm::normalize(glm::vec3(derecha.x, 0.0f, derecha.z));
        nuevaPos -= derechaHorizontal * cameraSpeed;
        mover = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 derecha = glm::normalize(glm::cross(cameraFront, cameraUp));
        glm::vec3 derechaHorizontal = glm::normalize(glm::vec3(derecha.x, 0.0f, derecha.z));
        nuevaPos += derechaHorizontal * cameraSpeed;
        mover = true;
    }

    if (mover) {
        if (verificarColisionParedes(nuevaPos) && !escenario.verificarColisionObjetos(nuevaPos, RADIO_JUGADOR)) {
            cameraPos = nuevaPos;
            cameraPos.y = ALTURA_JUGADOR;
        }
    }

    // CONTROL DE PASOS
    static float tiempoAcumulado = 0.0f;
    if (mover) {
        tiempoAcumulado += deltaTime;
        if (tiempoAcumulado >= intervaloPasos) {
            tiempoAcumulado = 0.0f;
            reproducirPaso();
        }
    } else {
        tiempoAcumulado = 0.0f;
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
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
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bodega del Terror", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // ========================================
    // CARGAR Y REPRODUCIR MÚSICA
    // ========================================
    try {
        if (musicaFondo.openFromFile("sounds/Ambience/Ambient_sound.ogg")) {
            musicaFondo.setLoop(true);
            musicaFondo.setVolume(50.0f);
            musicaFondo.play();
            musicaReproduciendo = true;
            std::cout << "Música cargada y reproduciendo" << std::endl;
        } else {
            std::cout << "Error: No se pudo cargar el archivo de música" << std::endl;
        }

        /*if (bufferPuerta.loadFromFile("sounds/SFX/fahhh.wav")) {
            soundPuerta.setBuffer(bufferPuerta);
            soundPuerta.setVolume(70.0f);
            sonidoCargado = true;
            std::cout << "Sonido de puerta cargado correctamente" << std::endl;
        } else {
            std::cout << "Error: No se pudo cargar el sonido de puerta" << std::endl;
        }*/

        // Cargar sonido de palanca
        /*if (bufferPalanca.loadFromFile("sounds/SFX/lever.wav")) {
            soundPalanca.setBuffer(bufferPalanca);
            soundPalanca.setVolume(70.0f);
            sonidoPalancaCargado = true;
            std::cout << "Sonido de palanca cargado correctamente" << std::endl;
        } else {
            std::cout << "Error: No se pudo cargar el sonido de palanca" << std::endl;
        }*/

        if (bufferPaso1.loadFromFile("sounds/SFX/Pasos_1.wav")) {
            std::cout << "Sonido de paso 1 cargado correctamente" << std::endl;
        } else {
            std::cout << "Error: No se pudo cargar sounds/SFX/Pasos_1.wav" << std::endl;
        }

        if (bufferPaso2.loadFromFile("sounds/SFX/Pasos_2.wav")) {
            std::cout << "Sonido de paso 2 cargado correctamente" << std::endl;
        } else {
            std::cout << "Error: No se pudo cargar sounds/SFX/Pasos_2.wav" << std::endl;
        }

        if (bufferPaso3.loadFromFile("sounds/SFX/Pasos_3.wav")) {
            std::cout << "Sonido de paso 3 cargado correctamente" << std::endl;
        } else {
            std::cout << "Error: No se pudo cargar sounds/SFX/Pasos_3.wav" << std::endl;
        }

        soundPaso.setVolume(40.0f);

    } catch (const std::exception& e) {
        std::cout << "Error al cargar audio: " << e.what() << std::endl;
    }

    std::cout << "\n=== BODEGA DEL TERROR ===" << std::endl;
    std::cout << "WASD: Moverse (con sonido de pasos)" << std::endl;
    std::cout << "Ratón: Mirar alrededor" << std::endl;
    std::cout << "M: Pausar/Reanudar música" << std::endl;
    std::cout << "E: Abrir/Cerrar puerta / Activar palanca" << std::endl;
    std::cout << "F: Linterna" << std::endl;
    std::cout << "F11: Pantalla Completa" << std::endl;
    std::cout << "ESC: Salir" << std::endl;
    std::cout << "========================\n" << std::endl;


    std::vector<std::string> faces;
    faces.push_back("Textures/px.jpg");
    faces.push_back("Textures/nx.jpg");
    faces.push_back("Textures/py.jpg");
    faces.push_back("Textures/ny.jpg");
    faces.push_back("Textures/nz.jpg");
    faces.push_back("Textures/pz.jpg");

    Skybox mySkybox(faces);
    Escenario escenario;

    cameraPos = glm::vec3(0.0f, ALTURA_JUGADOR, 7.5f);
    yaw = -90.0f;
    pitch = 0.0f;


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, escenario, deltaTime);

        escenario.update(deltaTime);

        // ========================================
        // EVENTO DE SALIDA
        // ========================================
        if (!eventoSalidaActivado && jugadorEnZonaSalida(cameraPos)) {
            eventoSalidaActivado = true;
            puertaBloqueada = true;

            if (escenario.isPuertaAbierta()) {
                escenario.togglePuerta();
            }

            std::cout << "\n=== EVENTO ACTIVADO ===" << std::endl;
            std::cout << "La puerta se ha bloqueado..." << std::endl;
        }

        // ========================================
        // CONTROL DE PUERTA CON TECLA E
        // ========================================
        static bool ePresionada = false;

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            if (!ePresionada) {
                escenario.togglePuertaMadera(cameraPos);
                ePresionada = true;

                if (!puertaBloqueada) {
                    if (escenario.jugadorCercaDePuerta(cameraPos)) {
                        escenario.togglePuerta();

                        if (sonidoCargado) {
                            soundPuerta.play();
                        }

                        std::cout << "Puerta " << (escenario.isPuertaAbierta() ? "abierta" : "cerrada") << std::endl;
                    }
                } else {
                    if (escenario.jugadorCercaDePuerta(cameraPos)) {
                        std::cout << "La puerta esta bloqueada..." << std::endl;
                    }
                }
                ePresionada = true;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
            ePresionada = false;
        }

        // ========================================
        // CONTROL DE PALANCA CON TECLA E (cerca de la palanca)
        // ========================================
        static bool ePalancaPresionada = false;

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            if (!ePalancaPresionada) {
                if (escenario.jugadorCercaPalanca(cameraPos)) {
                    escenario.togglePalanca();

                    if (sonidoPalancaCargado) {
                        soundPalanca.play();
                    }

                    if (escenario.isPalancaActivada()) {
                        std::cout << "¡Palanca activada!" << std::endl;
                    } else {
                        std::cout << "Palanca desactivada" << std::endl;
                    }
                }
                ePalancaPresionada = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
            ePalancaPresionada = false;
        }

        // ========================================
        // CONTROL DE LINTERNA CON TECLA F
        // ========================================
        static bool flashlightOn = true;
        static bool lastFState = false;
        bool currentFState = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
        if (currentFState && !lastFState) {
            flashlightOn = !flashlightOn;
        }
        lastFState = currentFState;

        // ========================================
        // CONTROL DE PANTALLA COMPLETA CON F11
        // ========================================
        static bool lastF11State = false;
        bool currentF11State = glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;

        if (currentF11State && !lastF11State) {
            static bool isFullscreen = false;
            static int windowedX, windowedY, windowedWidth, windowedHeight;

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            if (!isFullscreen) {
                // Guardar la posición y tamaño actuales de la ventana
                glfwGetWindowPos(window, &windowedX, &windowedY);
                glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

                // Cambiar a pantalla completa
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                std::cout << "Pantalla completa activada" << std::endl;
            } else {
                // Restaurar a modo ventana usando los datos guardados
                glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
                std::cout << "Modo ventana activado" << std::endl;
            }
            isFullscreen = !isFullscreen;
        }
        lastF11State = currentF11State;

        escenario.setFlashlight(cameraPos, cameraFront, flashlightOn);

        glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                0.1f, 100.0f);

        mySkybox.render(view, projection);
        escenario.render(view, projection, cameraPos, (float)glfwGetTime());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    musicaFondo.stop();

    glfwTerminate();
    return 0;
}