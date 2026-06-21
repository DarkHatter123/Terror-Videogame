#include "Menu2D.h"
#include <chrono>

// Definición de vertices del menu 2D
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

// Vertices de la linterna (definición global)
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

// Definición de variables globales
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

GameState currentState = MENU_PRINCIPAL;
bool manualVisto = false;
float tiempoMensajeBloqueo = 0.0f;
const float DURACION_MENSAJE_BLOQUEO = 2.0f;

std::chrono::steady_clock::time_point loadingStartTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point lastPuntoTime = std::chrono::steady_clock::now();
int puntosIndex = 0;
bool luzBlancoAmarillentoEncendida = true;
std::chrono::steady_clock::time_point lastLuzBlink = std::chrono::steady_clock::now();

bool luzBlancaEncendida = true;
std::chrono::steady_clock::time_point lastBlinkTime = std::chrono::steady_clock::now();

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
    }
    else {
        std::cout << "Error al cargar textura: " << path << std::endl;
    }
    return textureID;
}

void inicializarMenu2D(Shader& shaderProgram) {
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
}

void actualizarParpadeoLuzCentral() {
    auto now = std::chrono::steady_clock::now();
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

void actualizarParpadeoLinternaCarga() {
    if (vbLinternaPtr) {
        glBindBuffer(GL_ARRAY_BUFFER, *vbLinternaPtr);
        if (luzBlancoAmarillentoEncendida) {
            verticesLinterna[12 * 6 + 3] = 1.0f; verticesLinterna[12 * 6 + 4] = 0.95f; verticesLinterna[12 * 6 + 5] = 0.7f;
            verticesLinterna[13 * 6 + 3] = 1.0f; verticesLinterna[13 * 6 + 4] = 0.95f; verticesLinterna[13 * 6 + 5] = 0.7f;
            verticesLinterna[14 * 6 + 3] = 1.0f; verticesLinterna[14 * 6 + 4] = 0.95f; verticesLinterna[14 * 6 + 5] = 0.7f;
            verticesLinterna[15 * 6 + 3] = 1.0f; verticesLinterna[15 * 6 + 4] = 0.95f; verticesLinterna[15 * 6 + 5] = 0.7f;
        }
        else {
            verticesLinterna[12 * 6 + 3] = 0.0f; verticesLinterna[12 * 6 + 4] = 0.0f; verticesLinterna[12 * 6 + 5] = 0.0f;
            verticesLinterna[13 * 6 + 3] = 0.0f; verticesLinterna[13 * 6 + 4] = 0.0f; verticesLinterna[13 * 6 + 5] = 0.0f;
            verticesLinterna[14 * 6 + 3] = 0.0f; verticesLinterna[14 * 6 + 4] = 0.0f; verticesLinterna[14 * 6 + 5] = 0.0f;
            verticesLinterna[15 * 6 + 3] = 0.0f; verticesLinterna[15 * 6 + 4] = 0.0f; verticesLinterna[15 * 6 + 5] = 0.0f;
        }
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesLinterna), verticesLinterna);
    }
}

void resetearColoresLinterna() {
    if (vbLinternaPtr) {
        glBindBuffer(GL_ARRAY_BUFFER, *vbLinternaPtr);
        verticesLinterna[12 * 6 + 3] = 1.0f; verticesLinterna[12 * 6 + 4] = 0.95f; verticesLinterna[12 * 6 + 5] = 0.7f;
        verticesLinterna[13 * 6 + 3] = 1.0f; verticesLinterna[13 * 6 + 4] = 0.95f; verticesLinterna[13 * 6 + 5] = 0.7f;
        verticesLinterna[14 * 6 + 3] = 1.0f; verticesLinterna[14 * 6 + 4] = 0.95f; verticesLinterna[14 * 6 + 5] = 0.7f;
        verticesLinterna[15 * 6 + 3] = 1.0f; verticesLinterna[15 * 6 + 4] = 0.95f; verticesLinterna[15 * 6 + 5] = 0.7f;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesLinterna), verticesLinterna);
    }
}

void dibujarMenu2D(Shader& shaderProgram, GLuint uniID, int windowWidth, int windowHeight, float escalaImGui) {
    (void)windowWidth;
    (void)windowHeight;
    (void)escalaImGui;

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