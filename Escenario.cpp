#include "Escenario.h"
#include <SOIL2/SOIL2.h>
#include <iostream>

float ancho = 20.0f;
float alto = 10.0f;
float profundo = 30.0f;

unsigned int cargarTextura(const char* path) {
    unsigned int textureID = SOIL_load_OGL_texture(
        path,
        SOIL_LOAD_RGB,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if (textureID == 0) {
        std::cout << "Error al cargar textura con SOIL2: " << path << std::endl;
        std::cout << "Razón: " << SOIL_last_result() << std::endl;
    } else {
        std::cout << "Textura cargada correctamente: " << path << std::endl;

        // Configurar parámetros de textura
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    return textureID;
}

Escenario::Escenario() {
    srand(static_cast<unsigned int>(time(nullptr)));
    shader = new Shader("shaders/escenario.vert", "shaders/escenario.frag");
    shaderLuz = new Shader("shaders/luz.vert", "shaders/luz.frag");
    flashlightData.on = false;
    flashlightData.position = glm::vec3(0.0f);
    flashlightData.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    puertaAbierta = false;
    anguloPuerta = 0.0f;
    indicePuertaNormal = -1;

    // Puerta de salida (cerca del cartel EXIT)
    Puerta puertaSalida(glm::vec3(-7.5f, -2.5f, 29.8f));
    puertaSalida.setTamanio(2.2f, 5.0f, 0.1f);
    puertasMadera.push_back(puertaSalida);
    texturaPuertaIndustrial = cargarTextura("Textures/Puerta(2).png");
    texturaPared = cargarTextura("Textures/Pared.png");
    texturaMarcoPuerta = cargarTextura("Textures/Marco_puerta.png");


    setupHabitacion();
    setupCuboUnitario();
    setupEsfera();
    crearLuces();
    crearPuertas();
    crearEstanteriasYCajas();
    crearPasilloRecto();
    crearNuevaArea();
}



Escenario::~Escenario() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &cuboVAO);
    glDeleteBuffers(1, &cuboVBO);
    glDeleteVertexArrays(1, &esferaVAO);
    glDeleteBuffers(1, &esferaVBO);
    glDeleteBuffers(1, &esferaEBO);
    delete shader;
    delete shaderLuz;
}

void Escenario::setupEsfera() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    float radio = 0.5f;
    int sectorCount = 20;
    int stackCount = 14;

    for(int i = 0; i <= stackCount; i++) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount;
        float xy = radio * cosf(stackAngle);
        float z = radio * sinf(stackAngle);

        for(int j = 0; j <= sectorCount; j++) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(x/radio);
            vertices.push_back(y/radio);
            vertices.push_back(z/radio);
        }
    }

    for(int i = 0; i < stackCount; i++) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;
        for(int j = 0; j < sectorCount; j++, k1++, k2++) {
            if(i != 0) {
                indices.push_back(k1); indices.push_back(k2); indices.push_back(k1 + 1);
            }
            if(i != (stackCount - 1)) {
                indices.push_back(k1 + 1); indices.push_back(k2); indices.push_back(k2 + 1);
            }
        }
    }
    esferaIndices = indices.size();

    glGenVertexArrays(1, &esferaVAO);
    glGenBuffers(1, &esferaVBO);
    glGenBuffers(1, &esferaEBO);

    glBindVertexArray(esferaVAO);
    glBindBuffer(GL_ARRAY_BUFFER, esferaVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esferaEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Escenario::setupCuboUnitario() {

    float vertices[] = {

        // POSICION              // NORMAL           // UV

        // Frente
        -0.5f,-0.5f, 0.5f,      0,0,1,             0,0,
         0.5f,-0.5f, 0.5f,      0,0,1,             1,0,
         0.5f, 0.5f, 0.5f,      0,0,1,             1,1,
        -0.5f, 0.5f, 0.5f,      0,0,1,             0,1,

        // Atrás
        -0.5f,-0.5f,-0.5f,      0,0,-1,            1,0,
         0.5f,-0.5f,-0.5f,      0,0,-1,            0,0,
         0.5f, 0.5f,-0.5f,      0,0,-1,            0,1,
        -0.5f, 0.5f,-0.5f,      0,0,-1,            1,1,

        // Izquierda
        -0.5f,-0.5f,-0.5f,     -1,0,0,             0,0,
        -0.5f,-0.5f, 0.5f,     -1,0,0,             1,0,
        -0.5f, 0.5f, 0.5f,     -1,0,0,             1,1,
        -0.5f, 0.5f,-0.5f,     -1,0,0,             0,1,

        // Derecha
         0.5f,-0.5f,-0.5f,      1,0,0,             1,0,
         0.5f,-0.5f, 0.5f,      1,0,0,             0,0,
         0.5f, 0.5f, 0.5f,      1,0,0,             0,1,
         0.5f, 0.5f,-0.5f,      1,0,0,             1,1,

        // Abajo
        -0.5f,-0.5f,-0.5f,      0,-1,0,            0,1,
         0.5f,-0.5f,-0.5f,      0,-1,0,            1,1,
         0.5f,-0.5f, 0.5f,      0,-1,0,            1,0,
        -0.5f,-0.5f, 0.5f,      0,-1,0,            0,0,

        // Arriba
        -0.5f,0.5f,-0.5f,       0,1,0,             0,0,
         0.5f,0.5f,-0.5f,       0,1,0,             1,0,
         0.5f,0.5f, 0.5f,       0,1,0,             1,1,
        -0.5f,0.5f, 0.5f,       0,1,0,             0,1
    };

    unsigned int indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    glGenVertexArrays(1, &cuboVAO);
    glGenBuffers(1, &cuboVBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(cuboVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cuboVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normales
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Escenario::setupHabitacion() {
    float vertices[] = {
        -ancho/2, -alto/2, -profundo/2,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
         ancho/2, -alto/2, -profundo/2,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
         ancho/2,  alto/2, -profundo/2,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -ancho/2,  alto/2, -profundo/2,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

        -ancho/2, -alto/2,  profundo/2,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
         ancho/2, -alto/2,  profundo/2,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
         ancho/2,  alto/2,  profundo/2,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        -ancho/2,  alto/2,  profundo/2,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,

        -ancho/2, -alto/2, -profundo/2,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -ancho/2, -alto/2,  profundo/2,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -ancho/2,  alto/2,  profundo/2,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -ancho/2,  alto/2, -profundo/2,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

         ancho/2, -alto/2, -profundo/2,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         ancho/2, -alto/2,  profundo/2,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         ancho/2,  alto/2,  profundo/2,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         ancho/2,  alto/2, -profundo/2,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        -ancho/2, -alto/2, -profundo/2,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
         ancho/2, -alto/2, -profundo/2,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         ancho/2, -alto/2,  profundo/2,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
        -ancho/2, -alto/2,  profundo/2,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,

        -ancho/2,  alto/2, -profundo/2,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
         ancho/2,  alto/2, -profundo/2,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         ancho/2,  alto/2,  profundo/2,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
        -ancho/2,  alto/2,  profundo/2,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f
    };

    unsigned int indices[] = {
        0,1,2, 2,3,0,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Escenario::togglePuertaMadera(glm::vec3 jugadorPos)
{
    for (auto& puerta : puertasMadera)
    {
        if (puerta.jugadorCerca(jugadorPos))
        {
            puerta.toggle();
        }
    }
}

void Escenario::crearLuces() {
    float posicionesLuz[][3] = {
        {-4.0f, 4.0f, -10.0f},
        {4.0f, 4.0f, -10.0f},
        {-4.0f, 4.0f, 0.0f},
        {4.0f, 4.0f, 0.0f},
        {-4.0f, 4.0f, 10.0f},
        {4.0f, 4.0f, 10.0f},
    };

    float alturaTecho = alto/2;

    for(int i = 0; i < 6; i++) {
        float lx = posicionesLuz[i][0];
        float ly = 2.5f;
        float lz = posicionesLuz[i][2];
        LuzPuntual luz;
        luz.posicion = glm::vec3(lx, ly - 0.6f, lz);
        luz.intensidad = 1.2f;
        luz.color = glm::vec3(0.6f, 0.5f, 0.4f);
        luz.parpadea = (i == 2 || i == 5);
        luz.direction = glm::vec3(0.0f, -1.0f, 0.0f);
        luz.cutOff = glm::cos(glm::radians(30.0f));
        luz.outerCutOff = glm::cos(glm::radians(45.0f));
        luz.tiempoParpadeo = 0.0f;
        luz.offsetParpadeo = (float)(rand() % 100) / 50.0f;
        luz.visible = false;  // ← CAMBIADO: las bombillas NO se dibujan
        luces.push_back(luz);
        lamparas.push_back(Lampara(glm::vec3(lx, ly + 0.05f, lz)));
        float yCasquillo = ly + 0.1f;
        float largoCable = alturaTecho - yCasquillo;

        ObjetoFisico cable;
        cable.posicion = glm::vec3(lx, (alturaTecho + yCasquillo)/2.0f, lz);
        cable.escala = glm::vec3(0.02f, largoCable, 0.02f);
        cable.color = glm::vec3(0.05f, 0.05f, 0.05f);
        cable.tieneColision = false; cable.esPuerta = false;
        objetos.push_back(cable);

        ObjetoFisico casquillo;
        casquillo.posicion = glm::vec3(lx, ly + 0.05f, lz);
        casquillo.escala = glm::vec3(0.12f, 0.08f, 0.12f);
        casquillo.color = glm::vec3(0.25f, 0.25f, 0.28f);
        casquillo.tieneColision = false; casquillo.esPuerta = false;
        objetos.push_back(casquillo);

        ObjetoFisico rosca;
        rosca.posicion = glm::vec3(lx, ly - 0.05f, lz);
        rosca.escala = glm::vec3(0.08f, 0.06f, 0.08f);
        rosca.color = glm::vec3(0.4f, 0.4f, 0.4f);
        rosca.tieneColision = false; rosca.esPuerta = false;
        objetos.push_back(rosca);
    }
}

void Escenario::crearPuertas() {
    float sueloY = -alto/2;
    float puertaGarajeAncho = 6.0f;
    float puertaGarajeAlto = 7.0f;
    float zParedTrasera = -profundo/2;
    float grosorMarco = 0.25f;

    ObjetoFisico marcoIzq;
    marcoIzq.posicion = glm::vec3(-puertaGarajeAncho/2 - grosorMarco/2, sueloY + puertaGarajeAlto/2, zParedTrasera);
    marcoIzq.escala = glm::vec3(grosorMarco, puertaGarajeAlto, grosorMarco);
    marcoIzq.color = glm::vec3(0.2f, 0.2f, 0.22f);
    marcoIzq.tieneColision = true; marcoIzq.esPuerta = false;
    objetos.push_back(marcoIzq);

    ObjetoFisico marcoDer;
    marcoDer.posicion = glm::vec3(puertaGarajeAncho/2 + grosorMarco/2, sueloY + puertaGarajeAlto/2, zParedTrasera);
    marcoDer.escala = glm::vec3(grosorMarco, puertaGarajeAlto, grosorMarco);
    marcoDer.color = glm::vec3(0.2f, 0.2f, 0.22f);
    marcoDer.tieneColision = true; marcoDer.esPuerta = false;
    objetos.push_back(marcoDer);

    ObjetoFisico marcoSup;
    marcoSup.posicion = glm::vec3(0.0f, sueloY + puertaGarajeAlto + grosorMarco/2, zParedTrasera);
    marcoSup.escala = glm::vec3(puertaGarajeAncho + grosorMarco*2, grosorMarco, grosorMarco);
    marcoSup.color = glm::vec3(0.2f, 0.2f, 0.22f);
    marcoSup.tieneColision = true; marcoSup.esPuerta = false;
    objetos.push_back(marcoSup);

    int numPaneles = 4;
    float altoPanel = puertaGarajeAlto / numPaneles;

    for(int p = 0; p < numPaneles; p++) {
        float yPanel = sueloY + altoPanel/2 + p * altoPanel;

        ObjetoFisico panel;
        panel.posicion = glm::vec3(0.0f, yPanel, zParedTrasera + 0.02f);
        panel.escala = glm::vec3(puertaGarajeAncho - 0.1f, altoPanel - 0.05f, 0.08f);
        panel.color = glm::vec3(0.35f, 0.37f, 0.4f);
        panel.tieneColision = true; panel.esPuerta = false;
        objetos.push_back(panel);

        if(p == 1 || p == 2) {
            for(int v = -1; v <= 1; v += 2) {
                ObjetoFisico ventana;
                ventana.posicion = glm::vec3(v * 1.8f, yPanel, zParedTrasera + 0.07f);
                ventana.escala = glm::vec3(1.5f, altoPanel * 0.6f, 0.02f);
                ventana.color = glm::vec3(0.6f, 0.7f, 0.8f);
                ventana.tieneColision = true; ventana.esPuerta = false;
                ventana.rotarConPuerta = true;
                objetos.push_back(ventana);
                objetosPuertaIndustrial.push_back(objetos.size() - 1);

                ObjetoFisico marcoV;
                marcoV.posicion = glm::vec3(v * 1.8f, yPanel, zParedTrasera + 0.07f);
                marcoV.escala = glm::vec3(1.6f, altoPanel * 0.7f, 0.03f);
                marcoV.color = glm::vec3(0.15f, 0.15f, 0.15f);
                marcoV.tieneColision = true; marcoV.esPuerta = false;
                objetos.push_back(marcoV);
            }
        }
    }
    glm::vec3 colorParedF = glm::vec3(0.25f, 0.25f, 0.28f);
    float zParedFrontal = profundo/2;

    // Pared frontal izquierda CON TEXTURA
    ObjetoFisico pFIzq;
    pFIzq.posicion = glm::vec3(-9.3f, 0.0f, zParedFrontal);
    pFIzq.escala = glm::vec3(1.4f, alto, 0.2f);
    pFIzq.color = colorParedF;
    pFIzq.tieneColision = true;
    pFIzq.esPuerta = false;
    pFIzq.tieneTextura = true;        // ← NUEVO
    pFIzq.texturaID = texturaPared;   // ← NUEVO
    objetos.push_back(pFIzq);

    // Pared frontal derecha CON TEXTURA
    ObjetoFisico pFDer;
    pFDer.posicion = glm::vec3(1.8f, 0.0f, zParedFrontal);
    pFDer.escala = glm::vec3(16.4f, alto, 0.2f);
    pFDer.color = colorParedF;
    pFDer.tieneColision = true;
    pFDer.esPuerta = false;
    pFDer.tieneTextura = true;        // ← NUEVO
    pFDer.texturaID = texturaPared;   // ← NUEVO
    objetos.push_back(pFDer);

    // Pared frontal superior (encima de la puerta) CON TEXTURA
    ObjetoFisico pFSup;
    pFSup.posicion = glm::vec3(-7.5f, 2.5f, zParedFrontal);
    pFSup.escala = glm::vec3(2.2f, 5.0f, 0.2f);
    pFSup.color = colorParedF;
    pFSup.tieneColision = true;
    pFSup.esPuerta = false;
    pFSup.tieneTextura = true;        // ← NUEVO
    pFSup.texturaID = texturaMarcoPuerta;   // ← NUEVO
    objetos.push_back(pFSup);
    float xPuertaNormal = -7.5f;
    posicionOriginalPuerta = glm::vec3(xPuertaNormal, sueloY, zParedFrontal - 0.04f);

    indicePuertaNormal = objetos.size() + 3;

    // AQUÍ SE UTILIZA LA NUEVA PUERTA INDUSTRIAL
    crearPuertaIndustrial(posicionOriginalPuerta, false, true);

    objetos[indicePuertaNormal].esPuerta = true;
}

void Escenario::crearEstanteriasYCajas() {
    float alturaEstante = 8.0f;
    float anchoEstante = 3.0f;
    float profundoEstante = 0.8f;
    float grosorMetal = 0.08f;
    float numEstantes = 6;
    float sueloY = -alto/2;
    glm::vec3 colorMetalOscuro = glm::vec3(0.25f, 0.28f, 0.32f);
    glm::vec3 colorMetalClaro = glm::vec3(0.45f, 0.48f, 0.52f);
    glm::vec3 colorEstante = glm::vec3(0.35f, 0.38f, 0.42f);
    float posicionesZ[] = {-10.0f, -5.0f, 0.0f, 5.0f, 10.0f};

    for(int fila = 0; fila < 5; fila++) {
        float z = posicionesZ[fila];

        for(int col = 0; col < 3; col++) {
            float x = -7.0f + col * 7.0f;

            for(int esquinaX = -1; esquinaX <= 1; esquinaX += 2) {
                for(int esquinaZ = -1; esquinaZ <= 1; esquinaZ += 2) {
                    ObjetoFisico poste;
                    poste.posicion = glm::vec3(x + esquinaX * anchoEstante/2.0f, sueloY + alturaEstante/2.0f, z + esquinaZ * profundoEstante/2.0f);
                    poste.escala = glm::vec3(grosorMetal, alturaEstante, grosorMetal);
                    poste.color = colorMetalOscuro;
                    poste.tieneColision = true; poste.esPuerta = false;
                    objetos.push_back(poste);
                }
            }

            for(int alturaT = 0; alturaT <= 1; alturaT++) {
                float yT = sueloY + (alturaT == 0 ? 0.1f : alturaEstante - 0.1f);
                for(int frente = -1; frente <= 1; frente += 2) {
                    ObjetoFisico t;
                    t.posicion = glm::vec3(x, yT, z + frente * profundoEstante/2.0f);
                    t.escala = glm::vec3(anchoEstante, grosorMetal, grosorMetal);
                    t.color = colorMetalClaro;
                    t.tieneColision = true; t.esPuerta = false;
                    objetos.push_back(t);
                }
                for(int lateral = -1; lateral <= 1; lateral += 2) {
                    ObjetoFisico t;
                    t.posicion = glm::vec3(x + lateral * anchoEstante/2.0f, yT, z);
                    t.escala = glm::vec3(grosorMetal, grosorMetal, profundoEstante);
                    t.color = colorMetalClaro;
                    t.tieneColision = true; t.esPuerta = false;
                    objetos.push_back(t);
                }
            }

            for(int nivel = 1; nivel <= numEstantes; nivel++) {
                float yEstante = sueloY + nivel * (alturaEstante / (numEstantes + 1));

                ObjetoFisico repisa;
                repisa.posicion = glm::vec3(x, yEstante, z);
                repisa.escala = glm::vec3(anchoEstante - 0.1f, grosorMetal * 1.5f, profundoEstante - 0.05f);
                repisa.color = colorEstante;
                repisa.tieneColision = true; repisa.esPuerta = false;
                objetos.push_back(repisa);

                int cajasPorNivel = 1 + (nivel % 2);
                for(int c = 0; c < cajasPorNivel; c++) {
                    ObjetoFisico caja;
                    caja.posicion = glm::vec3(x + (c - 0.25f) * 0.6f, yEstante + 0.2f, z);
                    caja.escala = glm::vec3(0.5f, 0.35f, 0.4f);
                    caja.color = glm::vec3(0.5f + (nivel*c)*0.03f, 0.3f + (nivel+c)*0.02f, 0.12f + nivel*0.02f);
                    caja.tieneColision = true; caja.esPuerta = false;
                    objetos.push_back(caja);
                }
            }
        }
    }

    float sueloY2 = -alto/2;
    glm::vec3 colorCajaBase = glm::vec3(0.55f, 0.35f, 0.15f);
    struct EsquinaInfo {
        float x, z;
        float dirX, dirZ;
    };

    EsquinaInfo esquinas[3] = {
        {-ancho/2, -profundo/2, 1.0f, 1.0f},
        { ancho/2, -profundo/2, -1.0f, 1.0f},
        { ancho/2,  profundo/2, -1.0f, -1.0f}
    };

    for(int i = 0; i < 3; i++) {
        float tamX = 2.5f;
        float tamY = 2.0f;
        float tamZ = 2.5f;

        float xBase = esquinas[i].x + esquinas[i].dirX * tamX/2.0f;
        float zBase = esquinas[i].z + esquinas[i].dirZ * tamZ/2.0f;

        for(int j = 0; j < 3; j++) {
            ObjetoFisico cajaGrande;
            float yCaja = sueloY2 + tamY/2.0f + j * tamY;
            cajaGrande.posicion = glm::vec3(xBase, yCaja, zBase);
            cajaGrande.escala = glm::vec3(tamX, tamY, tamZ);
            float brillo = 1.0f + j * 0.1f;
            cajaGrande.color = glm::vec3(
                colorCajaBase.r * brillo,
                colorCajaBase.g * brillo,
                colorCajaBase.b * brillo
            );
            cajaGrande.tieneColision = true;
            cajaGrande.esPuerta = false;
            objetos.push_back(cajaGrande);
        }
    }
}

void Escenario::crearPasilloRecto() {
    float sueloY = -alto / 2.0f;
    float techoY = alto / 2.0f;
    float xPuerta = -7.5f;
    float zInicio = profundo / 2.0f;
    float anchoP = 3.0f;
    float largoP = 15.0f;
    glm::vec3 colPared = glm::vec3(0.22f, 0.20f, 0.24f);

    ObjetoFisico sueloPasillo;
    sueloPasillo.posicion = glm::vec3(xPuerta, sueloY, zInicio + largoP / 2.0f);
    sueloPasillo.escala = glm::vec3(anchoP, 0.05f, largoP);
    sueloPasillo.color = glm::vec3(0.1f, 0.1f, 0.1f);
    sueloPasillo.tieneColision = true; sueloPasillo.esPuerta = false;
    objetos.push_back(sueloPasillo);

    ObjetoFisico techoPasillo = sueloPasillo;
    techoPasillo.posicion.y = techoY;
    techoPasillo.escala.y = 0.05f;
    techoPasillo.color = colPared;
    techoPasillo.tieneColision = true;
    objetos.push_back(techoPasillo);

    ObjetoFisico paredDerecha;
    paredDerecha.posicion = glm::vec3(xPuerta + anchoP/2.0f, 0.0f, zInicio + largoP / 2.0f);
    paredDerecha.escala = glm::vec3(0.1f, alto, largoP);
    paredDerecha.color = colPared;
    paredDerecha.tieneColision = true;
    paredDerecha.esPuerta = false;
    objetos.push_back(paredDerecha);

    ObjetoFisico paredIzquierda;
    paredIzquierda.posicion = glm::vec3(xPuerta - anchoP/2.0f, 0.0f, zInicio + largoP / 2.0f);
    paredIzquierda.escala = glm::vec3(0.1f, alto, largoP);
    paredIzquierda.color = colPared;
    paredIzquierda.tieneColision = true;
    paredIzquierda.esPuerta = false;
    objetos.push_back(paredIzquierda);

    float xFinal = xPuerta;
    float zFinal = zInicio + largoP;
    float pAncho = 2.2f;
    float pAlto = 5.0f;

    ObjetoFisico pFinalIzq;
    pFinalIzq.posicion = glm::vec3(xFinal - (anchoP + pAncho)/4.0f, 0.0f, zFinal);
    pFinalIzq.escala = glm::vec3((anchoP - pAncho)/2.0f, alto, 0.1f);
    pFinalIzq.color = colPared;
    pFinalIzq.tieneColision = true;
    pFinalIzq.esPuerta = false;
    objetos.push_back(pFinalIzq);

    ObjetoFisico pFinalDer;
    pFinalDer.posicion = glm::vec3(xFinal + (anchoP + pAncho)/4.0f, 0.0f, zFinal);
    pFinalDer.escala = glm::vec3((anchoP - pAncho)/2.0f, alto, 0.1f);
    pFinalDer.color = colPared;
    pFinalDer.tieneColision = true;
    pFinalDer.esPuerta = false;
    objetos.push_back(pFinalDer);

    ObjetoFisico pFinalSup;
    pFinalSup.posicion = glm::vec3(xFinal, sueloY + pAlto + (alto - pAlto)/2.0f, zFinal);
    pFinalSup.escala = glm::vec3(pAncho, alto - pAlto, 0.1f);
    pFinalSup.color = colPared;
    pFinalSup.tieneColision = true;
    pFinalSup.esPuerta = false;
    objetos.push_back(pFinalSup);
    // En crearPasilloRecto(), agrega esta línea después de pFinalSup:

    // ==========================================
    // LUCES DEL PASILLO (con accesorios)
    // ==========================================
    float alturaLucesP = 4.2f;
    float separacionLuces = 4.5f;

    for(int i = 0; i < 4; i++) {
        float zLuz = zInicio + 2.5f + i * separacionLuces;
        if(zLuz < zFinal - 1.0f) {
            crearLuzPasillo(glm::vec3(xPuerta, alturaLucesP, zLuz));
        }
    }

    // ==========================================
    // CARTEL EXIT (SOLO RECTÁNGULO VERDE)
    // ==========================================
    ObjetoFisico cartelExit;
    cartelExit.posicion = glm::vec3(xFinal, sueloY + pAlto + 0.8f, zFinal - 0.06f);
    cartelExit.escala = glm::vec3(0.8f, 0.4f, 0.05f);
    cartelExit.color = glm::vec3(0.0f, 0.9f, 0.2f);
    cartelExit.tieneColision = false;
    cartelExit.esPuerta = false;
    objetos.push_back(cartelExit);

    LuzPuntual luzExit;
    luzExit.posicion = cartelExit.posicion + glm::vec3(0.0f, 0.0f, -0.1f);
    luzExit.intensidad = 0.8f;
    luzExit.color = glm::vec3(0.1f, 1.0f, 0.2f);
    luzExit.parpadea = true;
    luzExit.direction = glm::vec3(0.0f, -0.2f, -1.0f);
    luzExit.cutOff = glm::cos(glm::radians(60.0f));
    luzExit.outerCutOff = glm::cos(glm::radians(75.0f));
    luzExit.offsetParpadeo = (float)(rand() % 100) / 50.0f;
    luzExit.visible = false;  // ← NUEVO: no dibujar bombilla
    luces.push_back(luzExit);
}

void Escenario::crearLuzPasillo(glm::vec3 pos) {
    float alturaTecho = alto / 2.0f;

    // Luz puntual
    LuzPuntual luz;
    luz.posicion = pos - glm::vec3(0.0f, 0.15f, 0.0f);
    luz.intensidad = 1.0f;
    luz.color = glm::vec3(0.6f, 0.5f, 0.4f);
    luz.parpadea = (rand() % 10 > 7);
    luz.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    luz.cutOff = glm::cos(glm::radians(30.0f));
    luz.outerCutOff = glm::cos(glm::radians(45.0f));
    luz.offsetParpadeo = (float)(rand() % 100) / 50.0f;
    luces.push_back(luz);

    // LÁMPARA/CONO (accesorio visible)
    lamparas.push_back(Lampara(pos + glm::vec3(0.0f, 0.05f, 0.0f)));

    // Cable
    float yCasquillo = pos.y + 0.05f;
    ObjetoFisico cable;
    cable.posicion = glm::vec3(pos.x, (alturaTecho + yCasquillo) / 2.0f, pos.z);
    cable.escala = glm::vec3(0.02f, alturaTecho - yCasquillo, 0.02f);
    cable.color = glm::vec3(0.05f, 0.05f, 0.05f);
    cable.tieneColision = false; cable.esPuerta = false;
    objetos.push_back(cable);

    // Casquillo
    ObjetoFisico casquillo;
    casquillo.posicion = glm::vec3(pos.x, pos.y + 0.05f, pos.z);
    casquillo.escala = glm::vec3(0.12f, 0.08f, 0.12f);
    casquillo.color = glm::vec3(0.25f, 0.25f, 0.28f);
    casquillo.tieneColision = false; casquillo.esPuerta = false;
    objetos.push_back(casquillo);
}
void Escenario::crearPuertaEnPosicion(glm::vec3 pos, bool rotada, bool conColision) {
    float pAncho = 2.2f; float pAlto = 5.0f;
    glm::vec3 colorM = glm::vec3(0.15f, 0.10f, 0.06f);
    float grosorMarco = 0.12f;

    ObjetoFisico m1;
    m1.posicion = pos + (rotada ? glm::vec3(0, pAlto/2, -pAncho/2 - grosorMarco/2) : glm::vec3(-pAncho/2 - grosorMarco/2, pAlto/2, 0));
    m1.escala = rotada ? glm::vec3(grosorMarco, pAlto, grosorMarco) : glm::vec3(grosorMarco, pAlto, grosorMarco);
    m1.color = colorM; m1.tieneColision = conColision; m1.esPuerta = false;
    objetos.push_back(m1);

    ObjetoFisico m2;
    m2.posicion = pos + (rotada ? glm::vec3(0, pAlto/2, pAncho/2 + grosorMarco/2) : glm::vec3(pAncho/2 + grosorMarco/2, pAlto/2, 0));
    m2.escala = rotada ? glm::vec3(grosorMarco, pAlto, grosorMarco) : glm::vec3(grosorMarco, pAlto, grosorMarco);
    m2.color = colorM; m2.tieneColision = conColision; m2.esPuerta = false;
    objetos.push_back(m2);

    ObjetoFisico m3;
    m3.posicion = pos + (rotada ? glm::vec3(0, pAlto + grosorMarco/2, 0) : glm::vec3(0, pAlto + grosorMarco/2, 0));
    m3.escala = rotada ? glm::vec3(grosorMarco, grosorMarco, pAncho + grosorMarco*2) : glm::vec3(pAncho + grosorMarco*2, grosorMarco, grosorMarco);
    m3.color = colorM; m3.tieneColision = conColision; m3.esPuerta = false;
    objetos.push_back(m3);



    for(int py = -1; py <= 1; py += 2) {
        for(int lado = -1; lado <= 1; lado += 2) {
            ObjetoFisico panelDeco;
            panelDeco.posicion = pos + (rotada ? glm::vec3(lado * 0.035f, pAlto/2 + py * 0.8f, 0) : glm::vec3(0, pAlto/2 + py * 0.8f, lado * 0.035f));
            panelDeco.escala = rotada ? glm::vec3(0.01f, 0.6f, pAncho * 0.7f) : glm::vec3(pAncho * 0.7f, 0.6f, 0.01f);
            panelDeco.color = glm::vec3(0.25f, 0.15f, 0.07f);
            panelDeco.tieneColision = false; panelDeco.esPuerta = false;
            objetos.push_back(panelDeco);
        }
    }

    for(int lado = -1; lado <= 1; lado += 2) {
        ObjetoFisico picaporte;
        picaporte.posicion = pos + (rotada ? glm::vec3(lado * 0.04f, 2.0f, pAncho/2 - 0.15f) : glm::vec3(pAncho/2 - 0.15f, 2.0f, lado * 0.04f));
        picaporte.escala = rotada ? glm::vec3(0.05f, 0.08f, 0.08f) : glm::vec3(0.08f, 0.08f, 0.05f);
        picaporte.color = glm::vec3(0.7f, 0.6f, 0.3f);
        picaporte.tieneColision = false; picaporte.esPuerta = false;
        objetos.push_back(picaporte);
    }
}

void Escenario::crearPuertaIndustrial(glm::vec3 pos, bool rotada, bool conColision) {
    float pAncho = 2.2f;
    float pAlto = 5.0f;
    float grosorMarco = 0.12f;

    glm::vec3 colorMarco = glm::vec3(0.25f, 0.25f, 0.27f);
    glm::vec3 colorPuerta = glm::vec3(0.45f, 0.45f, 0.48f);

    // Marcos (sin textura)
    ObjetoFisico m1;
    m1.posicion = pos + (rotada ? glm::vec3(0, pAlto/2, -pAncho/2 - grosorMarco/2) : glm::vec3(-pAncho/2 - grosorMarco/2, pAlto/2, 0));
    m1.escala = rotada ? glm::vec3(grosorMarco, pAlto, grosorMarco) : glm::vec3(grosorMarco, pAlto, grosorMarco);
    m1.color = colorMarco;
    m1.tieneColision = conColision;
    m1.esPuerta = false;
    m1.tieneTextura = false;  // Sin textura
    objetos.push_back(m1);

    ObjetoFisico m2;
    m2.posicion = pos + (rotada ? glm::vec3(0, pAlto/2, pAncho/2 + grosorMarco/2) : glm::vec3(pAncho/2 + grosorMarco/2, pAlto/2, 0));
    m2.escala = rotada ? glm::vec3(grosorMarco, pAlto, grosorMarco) : glm::vec3(grosorMarco, pAlto, grosorMarco);
    m2.color = colorMarco;
    m2.tieneColision = conColision;
    m2.esPuerta = false;
    m2.tieneTextura = false;
    objetos.push_back(m2);

    ObjetoFisico m3;
    m3.posicion = pos + (rotada ? glm::vec3(0, pAlto + grosorMarco/2, 0) : glm::vec3(0, pAlto + grosorMarco/2, 0));
    m3.escala = rotada ? glm::vec3(grosorMarco, grosorMarco, pAncho + grosorMarco*2) : glm::vec3(pAncho + grosorMarco*2, grosorMarco, grosorMarco);
    m3.color = colorMarco;
    m3.tieneColision = conColision;
    m3.esPuerta = false;
    m3.tieneTextura = false;
    objetos.push_back(m3);

    // Puerta principal CON TEXTURA
    ObjetoFisico p;
    p.posicion = pos + glm::vec3(0, pAlto/2, 0);
    p.escala = rotada ? glm::vec3(0.06f, pAlto, pAncho) : glm::vec3(pAncho, pAlto, 0.06f);
    p.color = colorPuerta;
    p.tieneColision = true;
    p.esPuerta = true;
    p.rotarConPuerta = true;
    p.tieneTextura = true;           // NUEVO
    p.texturaID = texturaPuertaIndustrial;  // NUEVO
    objetos.push_back(p);
    objetosPuertaIndustrial.push_back(objetos.size() - 1);
}

bool Escenario::isPuertaAbierta() const {
    return puertaAbierta;
}

bool Escenario::verificarColisionObjetos(glm::vec3 posicionJugador, float radioJugador) const {
    // Colisión con objetos normales
    for(const auto& obj : objetos) {
        if(!obj.tieneColision) continue;
        glm::vec3 minObj = obj.posicion - obj.escala / 2.0f - glm::vec3(radioJugador);
        glm::vec3 maxObj = obj.posicion + obj.escala / 2.0f + glm::vec3(radioJugador);
        if(posicionJugador.x >= minObj.x && posicionJugador.x <= maxObj.x &&
           posicionJugador.y >= minObj.y && posicionJugador.y <= maxObj.y &&
           posicionJugador.z >= minObj.z && posicionJugador.z <= maxObj.z) {
            return true;
           }
    }

    // Colisión con puertas de madera
    for(const auto& puerta : puertasMadera) {
        if(puerta.verificarColision(posicionJugador, radioJugador)) {
            return true;
        }
    }

    return false;
}

bool Escenario::jugadorCercaPuerta(glm::vec3 posicionJugador)
{
    for (auto& puerta : puertasMadera)
    {
        float distancia =
            glm::distance(posicionJugador, puerta.getPosicion());

        if (distancia < 2.0f)
        {
            return true;
        }
    }

    return false;
}

void Escenario::configurarLuces(Shader& shader, float tiempo) {
    shader.use();
    shader.setInt("numSpotLights", (int)luces.size());

    for (size_t i = 0; i < luces.size(); i++) {
        std::string base = "spotLights[" + std::to_string(i) + "]";

        float intensidad = luces[i].intensidad;
        if (luces[i].parpadea) {
            float parpadeo = sin(tiempo * 7.0f + luces[i].offsetParpadeo) * 0.5f + 0.5f;
            intensidad *= 0.4f + parpadeo * 0.6f;
            if (parpadeo < 0.2f) intensidad = 0.15f;
        }

        glm::vec3 colorAjustado = luces[i].color * intensidad;
        shader.setVec3(base + ".position", luces[i].posicion);
        shader.setVec3(base + ".direction", luces[i].direction);
        shader.setFloat(base + ".cutOff", luces[i].cutOff);
        shader.setFloat(base + ".outerCutOff", luces[i].outerCutOff);
        shader.setVec3(base + ".ambient", colorAjustado * 0.03f);
        shader.setVec3(base + ".diffuse", colorAjustado * 1.0f);
        shader.setVec3(base + ".specular", glm::vec3(0.0f));
        shader.setFloat(base + ".constant", 1.0f);
        shader.setFloat(base + ".linear", 0.09f);
        shader.setFloat(base + ".quadratic", 0.032f);
    }
}

void Escenario::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, float tiempo) {
    glDisable(GL_CULL_FACE);

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("viewPos", cameraPos);
    configurarLuces(*shader, tiempo);
    configurarLinterna(*shader);

    // Colores para suelo y techo
    glm::vec3 coloresParedes[] = {
        glm::vec3(0.20f, 0.20f, 0.22f),  // [0] Pared TRASERA (con textura)
        glm::vec3(0.22f, 0.20f, 0.24f),  // [1] Pared IZQUIERDA (con textura)
        glm::vec3(0.22f, 0.20f, 0.24f),  // [2] Pared DERECHA (con textura)
        glm::vec3(0.30f, 0.28f, 0.26f),  // [3] SUELO (color sólido)
        glm::vec3(0.18f, 0.18f, 0.20f)   // [4] TECHO (color sólido)
    };

    glBindVertexArray(VAO);

    // ==========================================
    // PAREDES CON TEXTURA (índices 0, 1, 2)
    // ==========================================
    shader->setBool("usarTextura", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaPared);
    shader->setInt("textura", 0);

    for(int i = 0; i < 3; i++) {
        shader->setMat4("model", glm::mat4(1.0f));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));
    }

    // ==========================================
    // SUELO Y TECHO SIN TEXTURA (índices 3 y 4)
    // ==========================================
    for(int i = 3; i < 5; i++) {
        shader->setBool("usarTextura", false);
        shader->setVec3("objectColor", coloresParedes[i]);
        shader->setMat4("model", glm::mat4(1.0f));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));
    }

    glBindVertexArray(0);

    // ==========================================
    // RESTO DE OBJETOS
    // ==========================================
    glBindVertexArray(cuboVAO);
    for(const auto& obj : objetos) {
        glm::mat4 modelObj = glm::mat4(1.0f);
        modelObj = glm::translate(modelObj, obj.posicion);

        if(obj.rotarConPuerta) {
            glm::vec3 pivote(-obj.escala.x / 2.0f, 0.0f, 0.0f);
            modelObj = glm::translate(modelObj, pivote);
            modelObj = glm::rotate(modelObj, glm::radians(obj.rotacionY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelObj = glm::translate(modelObj, -pivote);
        }

        modelObj = glm::scale(modelObj, obj.escala);
        shader->setMat4("model", modelObj);

        if (obj.tieneTextura) {
            shader->setBool("usarTextura", true);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, obj.texturaID);
            shader->setInt("textura", 0);
        } else {
            shader->setBool("usarTextura", false);
            shader->setVec3("objectColor", obj.color);
        }

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // Dibujar objetos (con o sin textura)
    glBindVertexArray(cuboVAO);
    for(const auto& obj : objetos) {
        glm::mat4 modelObj = glm::mat4(1.0f);
        modelObj = glm::translate(modelObj, obj.posicion);

        if(obj.rotarConPuerta) {
            glm::vec3 pivote(-obj.escala.x / 2.0f, 0.0f, 0.0f);
            modelObj = glm::translate(modelObj, pivote);
            modelObj = glm::rotate(modelObj, glm::radians(obj.rotacionY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelObj = glm::translate(modelObj, -pivote);
        }

        modelObj = glm::scale(modelObj, obj.escala);
        shader->setMat4("model", modelObj);

        // Verificar si el objeto tiene textura
        if (obj.tieneTextura) {
            shader->setBool("usarTextura", true);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, obj.texturaID);
            shader->setInt("textura", 0);
        } else {
            shader->setBool("usarTextura", false);
            shader->setVec3("objectColor", obj.color);
        }

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // Lámparas
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    for (auto& lampara : lamparas) {
        lampara.render(*shader);
    }

    // Bombillas (usando shaderLuz)
    shaderLuz->use();
    shaderLuz->setMat4("view", view);
    shaderLuz->setMat4("projection", projection);

    glBindVertexArray(esferaVAO);
    for(const auto& luz : luces) {
        if (!luz.visible) continue;

        float intensidad = luz.intensidad;
        if(luz.parpadea) {
            float parpadeo = sin(tiempo * 7.0f + luz.offsetParpadeo) * 0.5f + 0.5f;
            intensidad *= 0.3f + parpadeo * 0.7f;
        }

        glm::mat4 modelLuz = glm::mat4(1.0f);
        modelLuz = glm::translate(modelLuz, luz.posicion - glm::vec3(0.0f, 0.15f, 0.0f));
        modelLuz = glm::scale(modelLuz, glm::vec3(0.1f, 0.14f, 0.1f));
        shaderLuz->setMat4("model", modelLuz);
        shaderLuz->setVec3("colorLuz", luz.color * glm::min(intensidad, 1.5f));
        shaderLuz->setFloat("brillo", 3.0f);
        glDrawElements(GL_TRIANGLES, esferaIndices, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);

    // Puertas de madera
    for (auto& puerta : puertasMadera) {
        puerta.render(shader->ID);
    }
}

void Escenario::renderPuertasMadera(const glm::mat4& view, const glm::mat4& projection, Shader* shader) {
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    for (auto& puerta : puertasMadera) {
        puerta.render(shader->ID);
    }
}

void Escenario::setFlashlight(const glm::vec3& pos, const glm::vec3& dir, bool on) {
    flashlightData.position = pos;
    flashlightData.direction = dir;
    flashlightData.on = on;
}

void Escenario::configurarLinterna(Shader& shader) {
    shader.use();
    shader.setBool("flashlightOn", flashlightData.on);
    if (!flashlightData.on) return;
    shader.setVec3("flashlight.position", flashlightData.position);
    shader.setVec3("flashlight.direction", flashlightData.direction);
    shader.setVec3("flashlight.ambient", glm::vec3(0.02f));
    shader.setVec3("flashlight.diffuse", glm::vec3(1.2f));
    shader.setVec3("flashlight.specular", glm::vec3(0.1f));
    shader.setFloat("flashlight.constant", 1.0f);
    shader.setFloat("flashlight.linear", 0.09f);
    shader.setFloat("flashlight.quadratic", 0.032f);
    float cutOff = glm::cos(glm::radians(20.0f));
    float outerCutOff = glm::cos(glm::radians(25.0f));
    shader.setFloat("flashlight.cutOff", cutOff);
    shader.setFloat("flashlight.outerCutOff", outerCutOff);
}

void Escenario::update(float deltaTime) {

    float velocidad = 120.0f;

    if (puertaAbierta) {

        anguloPuerta += velocidad * deltaTime;

        if (anguloPuerta > 90.0f)
            anguloPuerta = 90.0f;
    }
    else {

        anguloPuerta -= velocidad * deltaTime;

        if (anguloPuerta < 0.0f)
            anguloPuerta = 0.0f;
    }

    for (int idx : objetosPuertaIndustrial) {

        objetos[idx].rotacionY = -anguloPuerta;

        objetos[idx].tieneColision = (anguloPuerta < 70.0f);
    }
    for (auto& puerta : puertasMadera)
    {
        puerta.update(deltaTime);
    }
}

void Escenario::togglePuerta() {
    puertaAbierta = !puertaAbierta;
}

bool Escenario::jugadorCercaDePuerta(glm::vec3 posJugador) const {
    if (indicePuertaNormal == -1) return false;
    glm::vec2 jugador2D(posJugador.x, posJugador.z);
    glm::vec2 puerta2D(posicionOriginalPuerta.x, posicionOriginalPuerta.z);

    float distancia = glm::distance(jugador2D, puerta2D);
    return (distancia < 3.0f);
}

void Escenario::crearLuzRectangular(glm::vec3 pos) {
    ObjetoFisico panelLuz;
    panelLuz.posicion = pos;
    panelLuz.escala = glm::vec3(2.5f, 0.1f, 1.0f);
    panelLuz.color = glm::vec3(0.9f, 0.9f, 0.95f);
    panelLuz.tieneColision = false;
    panelLuz.esPuerta = false;
    objetos.push_back(panelLuz);

    LuzPuntual luz;
    luz.posicion = pos - glm::vec3(0.0f, 0.1f, 0.0f);
    luz.intensidad = 1.2f;
    luz.color = glm::vec3(0.7f, 0.7f, 0.8f);
    luz.parpadea = false;
    luz.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    luz.cutOff = glm::cos(glm::radians(45.0f));
    luz.outerCutOff = glm::cos(glm::radians(60.0f));
    luz.tiempoParpadeo = 0.0f;
    luz.offsetParpadeo = 0.0f;
    luces.push_back(luz);

}

void Escenario::crearNuevaArea() {
    float sueloY = -alto / 2.0f;
    float techoY = alto / 2.0f;

    float anchoSala = 14.0f;
    float largoSala = 14.0f;

    float zInicio = (profundo / 2.0f) + 15.0f;
    float xCentro = -7.5f;
    float zCentro = zInicio + (largoSala / 2.0f);

    glm::vec3 colPared = glm::vec3(0.24f, 0.22f, 0.26f);

    ObjetoFisico suelo;
    suelo.posicion = glm::vec3(xCentro, sueloY, zCentro);
    suelo.escala = glm::vec3(anchoSala, 0.05f, largoSala);
    suelo.color = glm::vec3(0.12f, 0.12f, 0.12f);
    suelo.tieneColision = true; suelo.esPuerta = false;
    objetos.push_back(suelo);

    ObjetoFisico techo = suelo;
    techo.posicion.y = techoY;
    techo.color = colPared;
    objetos.push_back(techo);

    ObjetoFisico pDer;
    pDer.posicion = glm::vec3(xCentro - anchoSala/2.0f, 0.0f, zCentro);
    pDer.escala = glm::vec3(0.2f, alto, largoSala);
    pDer.color = colPared; pDer.tieneColision = true; pDer.esPuerta = false;
    objetos.push_back(pDer);

    float pAncho = 2.2f; float pAlto = 5.0f;
    float largoParedMitad = (largoSala - pAncho) / 2.0f;

    ObjetoFisico pIzq1;
    pIzq1.posicion = glm::vec3(xCentro + anchoSala/2.0f, 0.0f, zInicio + largoParedMitad/2.0f);
    pIzq1.escala = glm::vec3(0.2f, alto, largoParedMitad);
    pIzq1.color = colPared; pIzq1.tieneColision = true; pIzq1.esPuerta = false;
    objetos.push_back(pIzq1);

    ObjetoFisico pIzq2;
    pIzq2.posicion = glm::vec3(xCentro + anchoSala/2.0f, 0.0f, zCentro + pAncho/2.0f + largoParedMitad/2.0f);
    pIzq2.escala = glm::vec3(0.2f, alto, largoParedMitad);
    pIzq2.color = colPared; pIzq2.tieneColision = true; pIzq2.esPuerta = false;
    objetos.push_back(pIzq2);

    ObjetoFisico pIzqSup;
    pIzqSup.posicion = glm::vec3(xCentro + anchoSala/2.0f, sueloY + pAlto + (alto - pAlto)/2.0f, zCentro);
    pIzqSup.escala = glm::vec3(0.2f, alto - pAlto, pAncho);
    pIzqSup.color = colPared; pIzqSup.tieneColision = true; pIzqSup.esPuerta = false;
    objetos.push_back(pIzqSup);

    // Puerta 1: Pared derecha (mirando desde el pasillo)
    puertasMadera.push_back(Puerta(glm::vec3(xCentro + anchoSala/2.0f + 0.1f, sueloY + 1.5f, zCentro), 2.2f, 7.0f, 0.1f, 90.0f));

    //crearPuertaEnPosicion(glm::vec3(xCentro + anchoSala/2.0f, sueloY, zCentro), true, false);

    float anchoParedMitad = (anchoSala - pAncho) / 2.0f;

    ObjetoFisico pFrente1;
    pFrente1.posicion = glm::vec3(xCentro - pAncho/2.0f - anchoParedMitad/2.0f, 0.0f, zInicio + largoSala);
    pFrente1.escala = glm::vec3(anchoParedMitad, alto, 0.2f);
    pFrente1.color = colPared; pFrente1.tieneColision = true; pFrente1.esPuerta = false;
    objetos.push_back(pFrente1);

    ObjetoFisico pFrente2;
    pFrente2.posicion = glm::vec3(xCentro + pAncho/2.0f + anchoParedMitad/2.0f, 0.0f, zInicio + largoSala);
    pFrente2.escala = glm::vec3(anchoParedMitad, alto, 0.2f);
    pFrente2.color = colPared; pFrente2.tieneColision = true; pFrente2.esPuerta = false;
    objetos.push_back(pFrente2);

    ObjetoFisico pFrenteSup;
    pFrenteSup.posicion = glm::vec3(xCentro, sueloY + pAlto + (alto - pAlto)/2.0f, zInicio + largoSala);
    pFrenteSup.escala = glm::vec3(pAncho, alto - pAlto, 0.2f);
    pFrenteSup.color = colPared; pFrenteSup.tieneColision = true; pFrenteSup.esPuerta = false;
    objetos.push_back(pFrenteSup);

    // Puerta 2: Pared frontal (al fondo de la sala)
    puertasMadera.push_back(Puerta(glm::vec3(xCentro, sueloY + 1.5f, zInicio + largoSala - 0.1f), 2.2f, 7.0f, 0.1f, 0.0f));
    //crearPuertaEnPosicion(glm::vec3(xCentro, sueloY, zInicio + largoSala - 0.04f), false, false);

    float anchoPasillo = 3.0f;
    float anchoParedTraseraMitad = (anchoSala - anchoPasillo) / 2.0f;

    ObjetoFisico pAtras1;
    pAtras1.posicion = glm::vec3(xCentro - anchoPasillo/2.0f - anchoParedTraseraMitad/2.0f, 0.0f, zInicio);
    pAtras1.escala = glm::vec3(anchoParedTraseraMitad, alto, 0.2f);
    pAtras1.color = colPared; pAtras1.tieneColision = true; pAtras1.esPuerta = false;
    objetos.push_back(pAtras1);

    ObjetoFisico pAtras2;
    pAtras2.posicion = glm::vec3(xCentro + anchoPasillo/2.0f + anchoParedTraseraMitad/2.0f, 0.0f, zInicio);
    pAtras2.escala = glm::vec3(anchoParedTraseraMitad, alto, 0.2f);
    pAtras2.color = colPared; pAtras2.tieneColision = true; pAtras2.esPuerta = false;
    objetos.push_back(pAtras2);

    glm::vec3 colorCajaBase = glm::vec3(0.55f, 0.35f, 0.15f);
    float offsetC = 1.8f;

    glm::vec3 esquinas[4] = {
        glm::vec3(xCentro - anchoSala/2.0f + offsetC, 0, zInicio + offsetC),
        glm::vec3(xCentro + anchoSala/2.0f - offsetC, 0, zInicio + offsetC),
        glm::vec3(xCentro - anchoSala/2.0f + offsetC, 0, zInicio + largoSala - offsetC),
        glm::vec3(xCentro + anchoSala/2.0f - offsetC, 0, zInicio + largoSala - offsetC)
    };

    for(int i = 0; i < 4; i++) {
        glm::vec3 tamaños[3] = {
            glm::vec3(2.5f, 2.0f, 2.5f),
            glm::vec3(1.6f, 1.5f, 1.6f),
            glm::vec3(1.0f, 0.9f, 1.0f)
        };

        float yActual = sueloY;
        for(int j = 0; j < 3; j++) {
            ObjetoFisico caja;
            caja.escala = tamaños[j];
            caja.posicion = glm::vec3(esquinas[i].x, yActual + caja.escala.y / 2.0f, esquinas[i].z);

            yActual += caja.escala.y;

            float brillo = 1.0f + j * 0.15f;
            caja.color = glm::vec3(colorCajaBase.r * brillo, colorCajaBase.g * brillo, colorCajaBase.b * brillo);
            caja.tieneColision = true;
            caja.esPuerta = false;
            objetos.push_back(caja);
        }
    }

    float luzOffsetX = anchoSala / 3.5f;
    float luzOffsetZ = largoSala / 3.5f;
    float alturaLampara = techoY - 0.05f;

    crearLuzRectangular(glm::vec3(xCentro - luzOffsetX, alturaLampara, zCentro - luzOffsetZ));
    crearLuzRectangular(glm::vec3(xCentro + luzOffsetX, alturaLampara, zCentro - luzOffsetZ));
    crearLuzRectangular(glm::vec3(xCentro - luzOffsetX, alturaLampara, zCentro + luzOffsetZ));
    crearLuzRectangular(glm::vec3(xCentro + luzOffsetX, alturaLampara, zCentro + luzOffsetZ));
}