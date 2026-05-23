#include "Skybox.h"

Skybox::Skybox(float radio) {
    generarEsfera(radio, 36, 18);
    
    shader = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
    
    // Configurar buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    // Posición (solo necesitamos posición para skybox)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete shader;
}

void Skybox::generarEsfera(float radio, int sectorCount, int stackCount) {
    vertices.clear();
    indices.clear();

    // Generar vértices
    for(int i = 0; i <= stackCount; i++) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount;
        float xy = radio * cosf(stackAngle);
        float z = radio * sinf(stackAngle);
        
        for(int j = 0; j <= sectorCount; j++) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            
            // Posición
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Normal invertida (apunta hacia adentro)
            vertices.push_back(-x/radio);
            vertices.push_back(-y/radio);
            vertices.push_back(-z/radio);
        }
    }
    
    // Generar índices para triángulos
    for(int i = 0; i < stackCount; i++) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;
        
        for(int j = 0; j < sectorCount; j++, k1++, k2++) {
            if(i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            
            if(i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    
    indexCount = indices.size();
}

void Skybox::render(const glm::mat4& view, const glm::mat4& projection) {
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    
    shader->use();
    
    // Eliminar traslación de la vista para que el cielo siga al jugador
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    
    shader->setMat4("view", viewNoTranslation);
    shader->setMat4("projection", projection);
    shader->setVec3("color", glm::vec3(0.05f, 0.05f, 0.1f)); // Color oscuro
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}