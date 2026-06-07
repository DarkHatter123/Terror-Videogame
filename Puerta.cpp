#include "Puerta.h"
#include "src/SOIL2/SOIL2.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Constructor por defecto
Puerta::Puerta(glm::vec3 pos)
{
    posicion = pos;
    ancho = 1.5f;
    alto = 3.0f;
    grosor = 0.2f;
    anguloActual = 0.0f;
    abierta = false;
    rotacionBase = 0.0f;
    tieneTextura = false;
    texturaID = 0;
    tieneTexturaPicaporte = false;
    texturaPicaporteID = 0;
}

// Constructor con tamaño personalizado
Puerta::Puerta(glm::vec3 pos, float anchoPuerta, float altoPuerta, float grosorPuerta)
{
    posicion = pos;
    ancho = anchoPuerta;
    alto = altoPuerta;
    grosor = grosorPuerta;
    anguloActual = 0.0f;
    abierta = false;
    rotacionBase = 0.0f;
    tieneTextura = false;
    texturaID = 0;
    tieneTexturaPicaporte = false;
    texturaPicaporteID = 0;
}

// Constructor con tamaño personalizado y rotación base
Puerta::Puerta(glm::vec3 pos, float anchoPuerta, float altoPuerta, float grosorPuerta, float rotacionBaseInicial)
{
    posicion = pos;
    ancho = anchoPuerta;
    alto = altoPuerta;
    grosor = grosorPuerta;
    anguloActual = 0.0f;
    abierta = false;
    rotacionBase = rotacionBaseInicial;
    tieneTextura = false;
    texturaID = 0;
    tieneTexturaPicaporte = false;
    texturaPicaporteID = 0;
}

void Puerta::setTamanio(float nuevoAncho, float nuevoAlto, float nuevoGrosor)
{
    ancho = nuevoAncho;
    alto = nuevoAlto;
    grosor = nuevoGrosor;
}

void Puerta::setTextura(const char* rutaTextura)
{
    texturaID = SOIL_load_OGL_texture(
        rutaTextura,
        SOIL_LOAD_RGB,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if (texturaID == 0) {
        std::cout << "Error al cargar textura de puerta: " << rutaTextura << std::endl;
        tieneTextura = false;
    } else {
        glBindTexture(GL_TEXTURE_2D, texturaID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        tieneTextura = true;
        std::cout << "Textura de puerta cargada: " << rutaTextura << std::endl;
    }
}

void Puerta::setTexturaPicaporte(const char* rutaTextura)
{
    texturaPicaporteID = SOIL_load_OGL_texture(
        rutaTextura,
        SOIL_LOAD_RGB,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if (texturaPicaporteID == 0) {
        std::cout << "Error al cargar textura de picaporte: " << rutaTextura << std::endl;
        tieneTexturaPicaporte = false;
    } else {
        glBindTexture(GL_TEXTURE_2D, texturaPicaporteID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        tieneTexturaPicaporte = true;
        std::cout << "Textura de picaporte cargada: " << rutaTextura << std::endl;
    }
}

void Puerta::toggle()
{
    abierta = !abierta;
}

bool Puerta::estaAbierta() const
{
    return abierta;
}

void Puerta::update(float deltaTime)
{
    float velocidad = 120.0f;
    if (abierta)
    {
        anguloActual += velocidad * deltaTime;
        if (anguloActual > 90.0f)
            anguloActual = 90.0f;
    }
    else
    {
        anguloActual -= velocidad * deltaTime;
        if (anguloActual < 0.0f)
            anguloActual = 0.0f;
    }
}

glm::mat4 Puerta::getModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, posicion);
    model = glm::rotate(model, glm::radians(rotacionBase), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-ancho / 2.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-anguloActual), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(ancho / 2.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(ancho, alto, grosor));
    return model;
}

bool Puerta::jugadorCerca(glm::vec3 jugadorPos)
{
    return glm::distance(jugadorPos, posicion) < 3.0f;
}

bool Puerta::verificarColision(glm::vec3 jugadorPos, float radioJugador) const
{
    if (anguloActual > 70.0f)
        return false;

    float minX = posicion.x - ancho / 2.0f;
    float maxX = posicion.x + ancho / 2.0f;
    float minZ = posicion.z - grosor / 2.0f;
    float maxZ = posicion.z + grosor / 2.0f;

    return (jugadorPos.x + radioJugador > minX &&
            jugadorPos.x - radioJugador < maxX &&
            jugadorPos.z + radioJugador > minZ &&
            jugadorPos.z - radioJugador < maxZ);
}

void Puerta::render(unsigned int shaderProgramID)
{
    glUseProgram(shaderProgramID);
    glDisable(GL_CULL_FACE);

    glm::mat4 model = getModelMatrix();
    unsigned int modelLoc = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    static unsigned int cubeVAO = 0, cubeVBO = 0, cubeEBO = 0;
    if (cubeVAO == 0) {
        float vertices[] = {
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
        };

        unsigned int indices[] = {
            0,1,2, 2,3,0, 4,5,6, 6,7,4,
            8,9,10, 10,11,8, 12,13,14, 14,15,12,
            16,17,18, 18,19,16, 20,21,22, 22,23,20
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glGenBuffers(1, &cubeEBO);

        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    // Configurar textura de la puerta
    if (tieneTextura) {
        glUniform1i(glGetUniformLocation(shaderProgramID, "usarTextura"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaID);
        glUniform1i(glGetUniformLocation(shaderProgramID, "textura"), 0);
    } else {
        glUniform1i(glGetUniformLocation(shaderProgramID, "usarTextura"), 0);
        glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.45f, 0.25f, 0.1f);
    }

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Picaporte delantero con textura
    glm::mat4 pomoModel = getModelMatrix();
    float pomoOffsetX = 0.37f;
    float pomoOffsetY = 0.15f;
    float pomoOffsetZ = 0.52f;

    glm::mat4 pomoLocal = glm::mat4(1.0f);
    pomoLocal = glm::translate(pomoLocal, glm::vec3(pomoOffsetX, pomoOffsetY, pomoOffsetZ));
    pomoLocal = glm::scale(pomoLocal, glm::vec3(0.08f, 0.08f, 0.08f));

    glm::mat4 pomoFinal = pomoModel * pomoLocal;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &pomoFinal[0][0]);

    if (tieneTexturaPicaporte) {
        glUniform1i(glGetUniformLocation(shaderProgramID, "usarTextura"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaPicaporteID);
        glUniform1i(glGetUniformLocation(shaderProgramID, "textura"), 0);
    } else {
        glUniform1i(glGetUniformLocation(shaderProgramID, "usarTextura"), 0);
        glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.8f, 0.7f, 0.2f);
    }
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Picaporte trasero
    pomoLocal = glm::mat4(1.0f);
    pomoLocal = glm::translate(pomoLocal, glm::vec3(pomoOffsetX, pomoOffsetY, -pomoOffsetZ));
    pomoLocal = glm::scale(pomoLocal, glm::vec3(0.08f, 0.08f, 0.08f));

    pomoFinal = pomoModel * pomoLocal;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &pomoFinal[0][0]);

    if (tieneTexturaPicaporte) {
        glUniform1i(glGetUniformLocation(shaderProgramID, "usarTextura"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaPicaporteID);
        glUniform1i(glGetUniformLocation(shaderProgramID, "textura"), 0);
    } else {
        glUniform1i(glGetUniformLocation(shaderProgramID, "usarTextura"), 0);
        glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.8f, 0.7f, 0.2f);
    }
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);

    // Restaurar estado
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}