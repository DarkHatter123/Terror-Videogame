#include "Escenario.h"
#include "src/SOIL2/SOIL2.h"
#include "Boton.h"
#include "MonstruoManager.h"
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
    shaderMonstruo = new Shader("shaders/monstruo.vert", "shaders/monstruo.frag");
    flashlightData.on = false;
    flashlightData.position = glm::vec3(0.0f);
    flashlightData.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    puertaAbierta = false;
    anguloPuerta = 0.0f;
    indicePuertaNormal = -1;
    secuenciaActual = 0;
    puzzleResuelto = false;
    palancaActivable = false;
    puertaJefeBloqueada = true;
    puertaSalidaBloqueada = true;
    botonSalidaPresionado = false;

    // Inicializar monstruo
    monstruoManager = new MonstruoManager();
    monstruoManager->setPosicionAparicion(glm::vec3(9.5f, -2.5f, 28.0f));
    monstruoManager->setPuedeAtravesarPuertas(true);
    monstruoActivado = false;

    Puerta puertaSalida(glm::vec3(-7.5f, -2.5f, 29.8f));
    puertaSalida.setTamanio(2.2f, 5.0f, 0.1f);
    puertasMadera.push_back(puertaSalida);
    texturaPuertaIndustrial = cargarTextura("Textures/PuertaMadera.png");
    texturaPared = cargarTextura("Textures/Pared4.png");
    texturaPared2 = cargarTextura("textures/Pared2.png");
    texturaPared3 = cargarTextura(("textures/ParedOficina.png"));
    texturaMarcoPuerta = cargarTextura("Textures/Pared4.png");
    texturaCaja1 = cargarTextura("Textures/Carton1.png");
    texturaCaja2 = cargarTextura("Textures/Carton2.png");
    texturaCaja3 = cargarTextura("Textures/Carton3.png");
    texturaSuelo = cargarTextura("textures/Suelo.png");
    texturaSueloJefe = cargarTextura("textures/SueloJefe.png");
    texturaSueloAlfombra = cargarTextura("textures/alfombra.png");
    texturaLampara = cargarTextura("textures/Lampara.png");
    texturatecho = cargarTextura(("textures/Texturatecho2.png"));
    texturaBodega = cargarTextura("textures/ParedBodega.png");
    texturaMarcoBodega = cargarTextura("textures/Marco_bodega.png");
    texturaNota = cargarTextura("Textures/Nota.jpg");
    if (texturaNota == 0) std::cout << "Advertencia: No se pudo cargar Textures/Nota.jpg" << std::endl;

    puertaSalida.setTextura("Textures/PuertaMadera.png");
    puertaSalida.setTexturaPicaporte("Textures/Picaporte.png");
    puertasMadera.push_back(puertaSalida);

    for (auto& puerta : puertasMadera) {
        puerta.setTextura("Textures/PuertaMadera.png");
        puerta.setTexturaPicaporte("Textures/Picaporte.png");
    }

    anguloPalanca = 0.0f;
    palancaActivada = false;
    palancaAnimando = false;
    modeloPalancaPtr = nullptr;

    Bodega();
    Cajas();
    Bombillo();
    Luces();
    Puertas();
    Estanterias();
    PasilloBodega();
    AreaCajas();
    PasillosAreaCajas();
    SalaJefe();
    AreaSeguridad();
    PasillosAreaSeguridad();
    Recepcion();
    PasilloRecepcion();
    crearIndicadoresProgreso();
    modelosTieneColision.clear();

    // Notas
    agregarNota(glm::vec3(6.0f, -1.0f, 14.8f), glm::vec3(0.8f, 0.6f, 0.05f),
       "La salida principal está bloqueada.\n"
       "Debes activar la palanca en la sala de seguridad.\n\n"
       "Busca en la zona de oficinas.");
    agregarNota(glm::vec3(-7.5f, -2.80f, 64.5f), glm::vec3(1.0f, 0.05f, 0.7f),
    "El cargamento que vino tenía un hedor extraño \nno parece ser nada grave\n\n"
    "-Reporte de Empleado",
    3.0f);
    agregarNota(glm::vec3(21.0f, -2.40f, 60.0f), glm::vec3(0.8f, 0.05f, 0.6f),
        "Horario de limpieza:\nSe recomienda no entrar después de las 22:00 hrs.\n\n"
        "(Mancha de café)");

    // buttons
    agregarBoton(glm::vec3(-10.0f, -2.5f, 30.1f), glm::vec3(0.4f, 0.4f, 0.1f), glm::vec3(1.0f, 0.2f, 0.2f), 1);
    agregarBoton(glm::vec3(22.85f, -0.50f, 45.95f), glm::vec3(0.4f, 0.4f, 0.1f), glm::vec3(0.2f, 1.0f, 0.2f), 2, 3.0f);
    agregarBoton(glm::vec3(20.45f, -2.5f, 56.1f), glm::vec3(0.4f, 0.4f, 0.1f), glm::vec3(0.2f, 0.2f, 1.0f), 3);
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
    delete shaderMonstruo;
    delete monstruoManager;

}

void Escenario::Bombillo() {
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

void Escenario::Cajas() {
    float vertices[] = {
        -0.5f,-0.5f, 0.5f,      0,0,1,             0,0,
         0.5f,-0.5f, 0.5f,      0,0,1,             1,0,
         0.5f, 0.5f, 0.5f,      0,0,1,             1,1,
        -0.5f, 0.5f, 0.5f,      0,0,1,             0,1,

        -0.5f,-0.5f,-0.5f,      0,0,-1,            1,0,
         0.5f,-0.5f,-0.5f,      0,0,-1,            0,0,
         0.5f, 0.5f,-0.5f,      0,0,-1,            0,1,
        -0.5f, 0.5f,-0.5f,      0,0,-1,            1,1,

        -0.5f,-0.5f,-0.5f,     -1,0,0,             0,0,
        -0.5f,-0.5f, 0.5f,     -1,0,0,             1,0,
        -0.5f, 0.5f, 0.5f,     -1,0,0,             1,1,
        -0.5f, 0.5f,-0.5f,     -1,0,0,             0,1,

         0.5f,-0.5f,-0.5f,      1,0,0,             1,0,
         0.5f,-0.5f, 0.5f,      1,0,0,             0,0,
         0.5f, 0.5f, 0.5f,      1,0,0,             0,1,
         0.5f, 0.5f,-0.5f,      1,0,0,             1,1,

        -0.5f,-0.5f,-0.5f,      0,-1,0,            0,1,
         0.5f,-0.5f,-0.5f,      0,-1,0,            1,1,
         0.5f,-0.5f, 0.5f,      0,-1,0,            1,0,
        -0.5f,-0.5f, 0.5f,      0,-1,0,            0,0,

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Escenario::Bodega() {
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

        -ancho/2, -alto/2, -profundo/2,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
         ancho/2, -alto/2, -profundo/2,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
         ancho/2, -alto/2,  profundo/2,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
        -ancho/2, -alto/2,  profundo/2,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,

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

//Metodos del mounstro
void Escenario::inicializarMonstruo() {
    if (monstruoManager) {
        monstruoManager->inicializar();
        monstruoActivado = false;
    }
}

void Escenario::activarMonstruo(const glm::vec3& posicionInicial) {
    if (monstruoManager) {
        monstruoManager->activarMonstruoEn(posicionInicial);
        monstruoActivado = true;
        std::cout << "Monstruo activado en Escenario" << std::endl;
    }
}

void Escenario::actualizarMonstruo(float deltaTime, const glm::vec3& posJugador) {
    if (monstruoManager) {
        monstruoManager->actualizar(deltaTime, posJugador, 0.4f);
    }
}

void Escenario::renderMonstruo(Shader& shader) {
    if (monstruoManager && monstruoManager->isMonstruoActivo()) {
        monstruoManager->render(shader);
    }
}

bool Escenario::isJugadorAtrapado() const {
    return monstruoManager && monstruoManager->isJugadorAtrapado();
}

bool Escenario::isMonstruoActivo() const {
    return monstruoManager && monstruoManager->isMonstruoActivo();
}

void Escenario::liberarJugador() {
    if (monstruoManager) {
        monstruoManager->liberarJugador();
    }
}

void Escenario::setMonstruoActivadoPorPuzzle(bool activado) {
    if (monstruoManager) {
        monstruoManager->setMonstruoActivadoPorPuzzle(activado);
    }
}

bool Escenario::isMonstruoActivadoPorPuzzle() const {
    return monstruoManager && monstruoManager->isMonstruoActivadoPorPuzzle();
}

void Escenario::setMonstruoAtraviesaPuertas(bool puede) {
    if (monstruoManager) {
        monstruoManager->setPuedeAtravesarPuertas(puede);
    }
}


void Escenario::togglePuertaMadera(glm::vec3 jugadorPos) {
    // Si el monstruo esta activo, permitir que atraviese puertas
    if (monstruoManager && monstruoManager->isMonstruoActivo()) {
        // El monstruo puede atravesar puertas, asi que abrimos sin restricciones
        for (auto& puerta : puertasMadera) {
            if (puerta.jugadorCerca(jugadorPos)) {
                puerta.toggle();
            }
        }
        return;
    }

    for (auto& puerta : puertasMadera) {
        if (puerta.jugadorCerca(jugadorPos)) {
            glm::vec3 posPuerta = puerta.getPosicion();

            // Detectar puerta del jefe (coordenada Z ≈ 53.9)
            bool esPuertaJefe = (posPuerta.x > -7.6f && posPuerta.x < -7.4f &&
                                  posPuerta.z > 53.8f && posPuerta.z < 54.0f);

            // Detectar puerta de salida (coordenada Z ≈ 74.0, X ≈ 18.5)
            bool esPuertaSalida = (posPuerta.x > 18.4f && posPuerta.x < 18.6f &&
                                    posPuerta.z > 73.9f && posPuerta.z < 74.1f);

            if ((esPuertaJefe && puertaJefeBloqueada) || (esPuertaSalida && puertaSalidaBloqueada)) {
                std::cout << "La puerta está bloqueada. Necesitas activar la palanca." << std::endl;
                continue;
            }
            puerta.toggle();
        }
    }
}

void Escenario::Luces() {
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
        luz.visible = false;
        luz.diffuseStrength = 1.0f;
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

void Escenario::Puertas() {
    float sueloY = -alto/2;
    float puertaGarajeAncho = 6.0f;
    float puertaGarajeAlto = 7.0f;
    float zParedTrasera = -profundo/2;
    float grosorMarco = 0.25f;

    // MARCO IZQUIERDO CON TEXTURA PARED
    ObjetoFisico marcoIzq;
    marcoIzq.posicion = glm::vec3(-puertaGarajeAncho/2 - grosorMarco/2, sueloY + puertaGarajeAlto/2, zParedTrasera);
    marcoIzq.escala = glm::vec3(grosorMarco, puertaGarajeAlto, grosorMarco);
    marcoIzq.color = glm::vec3(0.2f, 0.2f, 0.22f);
    marcoIzq.tieneColision = true;
    marcoIzq.esPuerta = false;
    marcoIzq.tieneTextura = true;
    marcoIzq.texturaID = texturaMarcoBodega;
    objetos.push_back(marcoIzq);

    // MARCO DERECHO CON TEXTURA PARED
    ObjetoFisico marcoDer;
    marcoDer.posicion = glm::vec3(puertaGarajeAncho/2 + grosorMarco/2, sueloY + puertaGarajeAlto/2, zParedTrasera);
    marcoDer.escala = glm::vec3(grosorMarco, puertaGarajeAlto, grosorMarco);
    marcoDer.color = glm::vec3(0.2f, 0.2f, 0.22f);
    marcoDer.tieneColision = true;
    marcoDer.esPuerta = false;
    marcoDer.tieneTextura = true;
    marcoDer.texturaID = texturaMarcoBodega;
    objetos.push_back(marcoDer);

    // MARCO SUPERIOR CON TEXTURA PARED
    ObjetoFisico marcoSup;
    marcoSup.posicion = glm::vec3(0.0f, sueloY + puertaGarajeAlto + grosorMarco/2, zParedTrasera);
    marcoSup.escala = glm::vec3(puertaGarajeAncho + grosorMarco*2, grosorMarco, grosorMarco);
    marcoSup.color = glm::vec3(0.2f, 0.2f, 0.22f);
    marcoSup.tieneColision = true;
    marcoSup.esPuerta = false;
    marcoSup.tieneTextura = true;
    marcoSup.texturaID = texturaMarcoBodega;
    objetos.push_back(marcoSup);

    int numPaneles = 4;
    float altoPanel = puertaGarajeAlto / numPaneles;

    for(int p = 0; p < numPaneles; p++) {
        float yPanel = sueloY + altoPanel/2 + p * altoPanel;

        ObjetoFisico panel;
        panel.posicion = glm::vec3(0.0f, yPanel, zParedTrasera + 0.02f);
        panel.escala = glm::vec3(puertaGarajeAncho + 0.3f, altoPanel + 0.06f, 0.08f);
        panel.color = glm::vec3(0.35f, 0.37f, 0.4f);
        panel.tieneColision = true; panel.esPuerta = false;
        panel.tieneTextura = true;
        panel.texturaID = texturaBodega;
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

    ObjetoFisico pFIzq;
    pFIzq.posicion = glm::vec3(-9.3f, 0.0f, zParedFrontal);
    pFIzq.escala = glm::vec3(1.4f, alto, 0.2f);
    pFIzq.color = colorParedF;
    pFIzq.tieneColision = true;
    pFIzq.esPuerta = false;
    pFIzq.tieneTextura = true;
    pFIzq.texturaID = texturaPared;
    objetos.push_back(pFIzq);

    ObjetoFisico pFDer;
    pFDer.posicion = glm::vec3(1.8f, 0.0f, zParedFrontal);
    pFDer.escala = glm::vec3(16.4f, alto, 0.2f);
    pFDer.color = colorParedF;
    pFDer.tieneColision = true;
    pFDer.esPuerta = false;
    pFDer.tieneTextura = true;
    pFDer.texturaID = texturaPared;
    objetos.push_back(pFDer);

    ObjetoFisico pFSup;
    pFSup.posicion = glm::vec3(-7.5f, 2.5f, zParedFrontal);
    pFSup.escala = glm::vec3(2.2f, 5.0f, 0.2f);
    pFSup.color = colorParedF;
    pFSup.tieneColision = true;
    pFSup.esPuerta = false;
    pFSup.tieneTextura = true;
    pFSup.texturaID = texturaMarcoPuerta;
    objetos.push_back(pFSup);
    float xPuertaNormal = -7.5f;
    posicionOriginalPuerta = glm::vec3(xPuertaNormal, sueloY, zParedFrontal - 0.04f);

    indicePuertaNormal = objetos.size() + 3;

    PuertaIndustrial(posicionOriginalPuerta, false, true);

    objetos[indicePuertaNormal].esPuerta = true;
}

void Escenario::Estanterias() {
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
                    poste.tieneColision = true;
                    poste.esPuerta = false;
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
                    t.tieneColision = true;
                    t.esPuerta = false;
                    objetos.push_back(t);
                }
                for(int lateral = -1; lateral <= 1; lateral += 2) {
                    ObjetoFisico t;
                    t.posicion = glm::vec3(x + lateral * anchoEstante/2.0f, yT, z);
                    t.escala = glm::vec3(grosorMetal, grosorMetal, profundoEstante);
                    t.color = colorMetalClaro;
                    t.tieneColision = true;
                    t.esPuerta = false;
                    objetos.push_back(t);
                }
            }

            for(int nivel = 1; nivel <= numEstantes; nivel++) {
                float yEstante = sueloY + nivel * (alturaEstante / (numEstantes + 1));

                ObjetoFisico repisa;
                repisa.posicion = glm::vec3(x, yEstante, z);
                repisa.escala = glm::vec3(anchoEstante - 0.1f, grosorMetal * 1.5f, profundoEstante - 0.05f);
                repisa.color = colorEstante;
                repisa.tieneColision = true;
                repisa.esPuerta = false;
                objetos.push_back(repisa);

                int cajasPorNivel = 1 + (nivel % 2);
                for(int c = 0; c < cajasPorNivel; c++) {
                    ObjetoFisico caja;
                    caja.posicion = glm::vec3(x + (c - 0.25f) * 0.6f, yEstante + 0.2f, z);
                    caja.escala = glm::vec3(0.5f, 0.35f, 0.4f);
                    caja.color = glm::vec3(0.5f + (nivel*c)*0.03f, 0.3f + (nivel+c)*0.02f, 0.12f + nivel*0.02f);
                    caja.tieneColision = true;
                    caja.esPuerta = false;
                    caja.tieneTextura = true;
                    int texturaIndex = (nivel + c) % 3;
                    if (texturaIndex == 0) {
                        caja.texturaID = texturaCaja1;
                    } else if (texturaIndex == 1) {
                        caja.texturaID = texturaCaja2;
                    } else {
                        caja.texturaID = texturaCaja3;
                    }
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
            cajaGrande.tieneTextura = true;
            int texturaIndex = (i + j) % 3;
            if (texturaIndex == 0) {
                cajaGrande.texturaID = texturaCaja1;
            } else if (texturaIndex == 1) {
                cajaGrande.texturaID = texturaCaja2;
            } else {
                cajaGrande.texturaID = texturaCaja3;
            }
            objetos.push_back(cajaGrande);
        }
    }

    unsigned int texturaMan = cargarTextura("textures/mapa.jpg");

    ObjetoFisico objetoManBodega;
    objetoManBodega.posicion = glm::vec3(-3.0f, -1.0f, 14.7f);
    objetoManBodega.escala = glm::vec3(2.5f, 2.5f, 0.05f);
    objetoManBodega.rotacionY = 0.0f;
    objetoManBodega.tieneTextura = true;
    objetoManBodega.texturaID = texturaMan;
    objetoManBodega.tieneColision = true;
    objetoManBodega.esPuerta = false;
    objetos.push_back(objetoManBodega);

    std::cout << "Objeto con textura man.png agregado en Bodega (pared derecha)" << std::endl;
}

void Escenario::PasilloBodega() {
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
    sueloPasillo.tieneColision = true;
    sueloPasillo.esPuerta = false;
    sueloPasillo.tieneTextura = true;
    sueloPasillo.texturaID = texturaSuelo;
    objetos.push_back(sueloPasillo);

    ObjetoFisico techoPasillo = sueloPasillo;
    techoPasillo.posicion.y = techoY;
    techoPasillo.escala.y = 0.05f;
    techoPasillo.color = colPared;
    techoPasillo.tieneColision = true;
    techoPasillo.tieneTextura = true;
    techoPasillo.texturaID = texturatecho;
    objetos.push_back(techoPasillo);

    ObjetoFisico paredDerecha;
    paredDerecha.posicion = glm::vec3(xPuerta + anchoP/2.0f, 0.0f, zInicio + largoP / 2.0f);
    paredDerecha.escala = glm::vec3(0.1f, alto, largoP);
    paredDerecha.color = colPared;
    paredDerecha.tieneColision = true;
    paredDerecha.esPuerta = false;
    paredDerecha.tieneTextura = true;
    paredDerecha.texturaID = texturaPared2;
    objetos.push_back(paredDerecha);

    ObjetoFisico paredIzquierda;
    paredIzquierda.posicion = glm::vec3(xPuerta - anchoP/2.0f, 0.0f, zInicio + largoP / 2.0f);
    paredIzquierda.escala = glm::vec3(0.1f, alto, largoP);
    paredIzquierda.color = colPared;
    paredIzquierda.tieneColision = true;
    paredIzquierda.esPuerta = false;
    paredIzquierda.tieneTextura = true;
    paredIzquierda.texturaID = texturaPared2;
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
    pFinalIzq.tieneTextura = true;
    pFinalIzq.texturaID = texturaPared2;
    objetos.push_back(pFinalIzq);

    ObjetoFisico pFinalDer;
    pFinalDer.posicion = glm::vec3(xFinal + (anchoP + pAncho)/4.0f, 0.0f, zFinal);
    pFinalDer.escala = glm::vec3((anchoP - pAncho)/2.0f, alto, 0.1f);
    pFinalDer.color = colPared;
    pFinalDer.tieneColision = true;
    pFinalDer.esPuerta = false;
    pFinalDer.tieneTextura = true;
    pFinalDer.texturaID = texturaPared2;
    objetos.push_back(pFinalDer);

    ObjetoFisico pFinalSup;
    pFinalSup.posicion = glm::vec3(xFinal, sueloY + pAlto + (alto - pAlto)/2.0f, zFinal);
    pFinalSup.escala = glm::vec3(pAncho, alto - pAlto, 0.1f);
    pFinalSup.color = colPared;
    pFinalSup.tieneColision = true;
    pFinalSup.esPuerta = false;
    pFinalSup.tieneTextura = true;
    pFinalSup.texturaID = texturaPared2;
    objetos.push_back(pFinalSup);

    float alturaLucesP = 4.2f;
    float separacionLuces = 4.5f;

    for(int i = 0; i < 4; i++) {
        float zLuz = zInicio + 2.5f + i * separacionLuces;
        if(zLuz < zFinal - 1.0f) {
            LucesPasillo(glm::vec3(xPuerta, alturaLucesP, zLuz));
        }
    }

    ObjetoFisico cartelExit;
    cartelExit.posicion = glm::vec3(xFinal, sueloY + pAlto + 0.8f, zFinal - 0.06f);
    cartelExit.escala = glm::vec3(0.8f, 0.4f, 0.05f);
    cartelExit.color = glm::vec3(0.0f, 0.9f, 0.2f);
    cartelExit.tieneColision = false;
    cartelExit.esPuerta = false;
    objetos.push_back(cartelExit);

    LuzPuntual luzExit;
    luzExit.posicion = cartelExit.posicion + glm::vec3(0.0f, 0.0f, -0.1f);
    luzExit.intensidad = 0.5f;
    luzExit.color = glm::vec3(0.1f, 1.0f, 0.2f);
    luzExit.parpadea = true;
    luzExit.direction = glm::vec3(0.0f, -0.2f, -1.0f);
    luzExit.cutOff = glm::cos(glm::radians(60.0f));
    luzExit.outerCutOff = glm::cos(glm::radians(75.0f));
    luzExit.offsetParpadeo = (float)(rand() % 100) / 50.0f;
    luzExit.visible = false;
    luzExit.diffuseStrength = 1.0f;
    luces.push_back(luzExit);
}

void Escenario::LucesPasillo(glm::vec3 pos) {
    float alturaTecho = alto / 2.0f;

    LuzPuntual luz;
    luz.posicion = pos - glm::vec3(0.0f, 0.15f, 0.0f);
    luz.intensidad = 0.75f;
    luz.color = glm::vec3(0.75f, 0.75f, 0.85f);
    luz.parpadea = false;
    luz.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    luz.cutOff = glm::cos(glm::radians(30.0f));
    luz.outerCutOff = glm::cos(glm::radians(45.0f));
    luz.offsetParpadeo = (float)(rand() % 100) / 50.0f;
    luz.diffuseStrength = 0.4f;
    luces.push_back(luz);

    lamparas.push_back(Lampara(pos + glm::vec3(0.0f, 0.05f, 0.0f)));

    float yCasquillo = pos.y + 0.05f;
    ObjetoFisico cable;
    cable.posicion = glm::vec3(pos.x, (alturaTecho + yCasquillo) / 2.0f, pos.z);
    cable.escala = glm::vec3(0.02f, alturaTecho - yCasquillo, 0.02f);
    cable.color = glm::vec3(0.05f, 0.05f, 0.05f);
    cable.tieneColision = false; cable.esPuerta = false;
    objetos.push_back(cable);

    ObjetoFisico casquillo;
    casquillo.posicion = glm::vec3(pos.x, pos.y + 0.05f, pos.z);
    casquillo.escala = glm::vec3(0.12f, 0.08f, 0.12f);
    casquillo.color = glm::vec3(0.25f, 0.25f, 0.28f);
    casquillo.tieneColision = false; casquillo.esPuerta = false;
    objetos.push_back(casquillo);
}

void Escenario::PuertasMadera(glm::vec3 pos, bool rotada, bool conColision) {
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

void Escenario::PuertaIndustrial(glm::vec3 pos, bool rotada, bool conColision) {
    float pAncho = 2.2f;
    float pAlto = 5.0f;
    float grosorMarco = 0.12f;

    glm::vec3 colorMarco = glm::vec3(0.25f, 0.25f, 0.27f);
    glm::vec3 colorPuerta = glm::vec3(0.45f, 0.45f, 0.48f);

    ObjetoFisico m1;
    m1.posicion = pos + (rotada ? glm::vec3(0, pAlto/2, -pAncho/2 - grosorMarco/2) : glm::vec3(-pAncho/2 - grosorMarco/2, pAlto/2, 0));
    m1.escala = rotada ? glm::vec3(grosorMarco, pAlto, grosorMarco) : glm::vec3(grosorMarco, pAlto, grosorMarco);
    m1.color = colorMarco;
    m1.tieneColision = conColision;
    m1.esPuerta = false;
    m1.tieneTextura = false;
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

    ObjetoFisico p;
    p.posicion = pos + glm::vec3(0, pAlto/2, 0);
    p.escala = rotada ? glm::vec3(0.06f, pAlto, pAncho) : glm::vec3(pAncho, pAlto, 0.06f);
    p.color = colorPuerta;
    p.tieneColision = true;
    p.esPuerta = true;
    p.rotarConPuerta = true;
    p.tieneTextura = true;
    p.texturaID = texturaPuertaIndustrial;
    objetos.push_back(p);
    objetosPuertaIndustrial.push_back(objetos.size() - 1);
}

bool Escenario::isPuertaAbierta() const {
    return puertaAbierta;
}

bool Escenario::verificarColisionObjetos(glm::vec3 posicionJugador, float radioJugador) const {
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

    for(const auto& puerta : puertasMadera) {
        if(puerta.verificarColision(posicionJugador, radioJugador)) {
            return true;
        }
    }

    return false;
}

bool Escenario::jugadorCercaPuerta(glm::vec3 posicionJugador) {
    for (auto& puerta : puertasMadera) {
        float distancia = glm::distance(posicionJugador, puerta.getPosicion());
        if (distancia < 2.0f) {
            glm::vec3 posPuerta = puerta.getPosicion();
            bool esPuertaJefe = (posPuerta.x > -8.0f && posPuerta.x < -7.0f && posPuerta.z > 64.0f && posPuerta.z < 65.0f);
            bool esPuertaSalida = (posPuerta.x > -8.0f && posPuerta.x < -7.0f && posPuerta.z > 29.0f && posPuerta.z < 30.5f);

            if ((esPuertaJefe && puertaJefeBloqueada) || (esPuertaSalida && puertaSalidaBloqueada)) {
                std::cout << "La puerta está bloqueada. Necesitas activar la palanca." << std::endl;
                return false;
            }
            return true;
        }
    }
    return false;
}

bool Escenario::jugadorCercaPalanca(glm::vec3 posJugador) const {
    if (!modeloPalancaPtr) return false;
    glm::vec2 jugador2D(posJugador.x, posJugador.z);
    glm::vec2 palanca2D(37.5f, 37.0f);
    float distancia = glm::distance(jugador2D, palanca2D);
    return (distancia < 2.0f);
}

void Escenario::togglePalanca() {
    if (palancaAnimando) return;
    if (!puzzleResuelto) {
        std::cout << "La palanca no tiene energía. Resuelve el puzzle de los botones de colores primero." << std::endl;
        return;
    }
    palancaActivada = !palancaActivada;
    palancaAnimando = true;
    std::cout << "Palanca " << (palancaActivada ? "activada" : "desactivada") << std::endl;

    if (palancaActivada) {
        abrirPuertaJefe();
        std::cout << "¡La energía ha desbloqueado la oficina del jefe!" << std::endl;
    }
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
        shader.setFloat(base + ".diffuseStrength", luces[i].diffuseStrength);
        shader.setVec3(base + ".position", luces[i].posicion);
        shader.setVec3(base + ".direction", luces[i].direction);
        shader.setFloat(base + ".cutOff", luces[i].cutOff);
        shader.setFloat(base + ".outerCutOff", luces[i].outerCutOff);
        shader.setVec3(base + ".ambient", colorAjustado * 0.03f);
        shader.setVec3(base + ".diffuse", colorAjustado * 1.0f * luces[i].diffuseStrength);
        shader.setVec3(base + ".specular", glm::vec3(0.0f));
        shader.setFloat(base + ".constant", 1.0f);
        shader.setFloat(base + ".linear", 0.09f);
        shader.setFloat(base + ".quadratic", 0.032f);
    }
}

void Escenario::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, float tiempo) {
    glDisable(GL_CULL_FACE);

    // ==================== CONFIGURACIÓN DEL SHADER PRINCIPAL ====================
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("viewPos", cameraPos);
    configurarLuces(*shader, tiempo);
    configurarLinterna(*shader);

    glm::vec3 coloresParedes[] = {
        glm::vec3(0.20f, 0.20f, 0.22f),
        glm::vec3(0.22f, 0.20f, 0.24f),
        glm::vec3(0.22f, 0.20f, 0.24f),
        glm::vec3(0.30f, 0.28f, 0.26f),
        glm::vec3(0.18f, 0.18f, 0.20f)
    };

    glBindVertexArray(VAO);

    // PAREDES CON TEXTURA (índices 0, 1, 2)
    shader->setBool("usarTextura", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaPared);
    shader->setInt("textura", 0);

    for(int i = 0; i < 3; i++) {
        shader->setMat4("model", glm::mat4(1.0f));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));
    }

    // SUELO CON TEXTURA (índice 3)
    shader->setBool("usarTextura", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaSuelo);
    shader->setInt("textura", 0);
    shader->setMat4("model", glm::mat4(1.0f));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(3 * 6 * sizeof(unsigned int)));

    // TECHO SIN TEXTURA (índice 4)
    shader->setBool("usarTextura", false);
    shader->setVec3("objectColor", coloresParedes[4]);
    shader->setMat4("model", glm::mat4(1.0f));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(4 * 6 * sizeof(unsigned int)));

    glBindVertexArray(0);

    // ==================== RESTO DE OBJETOS (cubos, cajas, indicadores, etc.) ====================
    glBindVertexArray(cuboVAO);
    for (const auto& obj : objetos) {
        if (!obj.visible) continue;

        glm::mat4 modelObj = glm::mat4(1.0f);
        modelObj = glm::translate(modelObj, obj.posicion);

        if (obj.esIndicador && obj.rotacionY != 0.0f) {
            modelObj = glm::rotate(modelObj, glm::radians(obj.rotacionY), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        if (obj.rotarConPuerta) {
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

    // ==================== NOTAS ====================
    renderNotas(*shader);

    // ==================== BOTONES ====================
    renderBotones(*shader);

    // ==================== LÁMPARAS ====================
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    for (auto& lampara : lamparas) {
        lampara.render(*shader);
    }

    // ==================== BOMBILLAS (luces puntuales visibles) ====================
    shaderLuz->use();
    shaderLuz->setMat4("view", view);
    shaderLuz->setMat4("projection", projection);

    glBindVertexArray(esferaVAO);
    for (const auto& luz : luces) {
        if (!luz.visible) continue;

        float intensidad = luz.intensidad;
        if (luz.parpadea) {
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

    // ==================== MODELOS EXTRA (OBJ) ====================
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    for (size_t i = 0; i < modelosExtra.size(); i++) {
        if (modelosExtra[i]) {
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, modelosPosiciones[i]);
            modelMat = glm::scale(modelMat, modelosEscalas[i]);

            shader->setMat4("model", modelMat);
            shader->setBool("usarTextura", true);

            modelosExtra[i]->Draw(*shader);
        }
    }

    //Moustro render
    if (monstruoManager && monstruoManager->isMonstruoActivo()) {
        shaderMonstruo->use();
        shaderMonstruo->setMat4("view", view);
        shaderMonstruo->setMat4("projection", projection);
        monstruoManager->render(*shaderMonstruo);
    }

    glEnable(GL_CULL_FACE);

    // ==================== PUERTAS DE MADERA ====================
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
    shader.setFloat("flashlight.diffuseStrength", 1.0f);
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
        if (anguloPuerta > 90.0f) anguloPuerta = 90.0f;
    } else {
        anguloPuerta -= velocidad * deltaTime;
        if (anguloPuerta < 0.0f) anguloPuerta = 0.0f;
    }

    for (int idx : objetosPuertaIndustrial) {
        objetos[idx].rotacionY = -anguloPuerta;
        objetos[idx].tieneColision = (anguloPuerta < 70.0f);
    }
    for (auto& puerta : puertasMadera) {
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

void Escenario::LucesRectangulares(glm::vec3 pos) {
    ObjetoFisico panelLuz;
    panelLuz.posicion = pos;
    panelLuz.escala = glm::vec3(2.5f, 0.1f, 1.0f);
    panelLuz.color = glm::vec3(0.9f, 0.9f, 0.95f);
    panelLuz.tieneColision = false;
    panelLuz.esPuerta = false;
    panelLuz.tieneTextura = true;
    panelLuz.texturaID = texturaLampara;
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
    luz.diffuseStrength = 0.4f;
    luz.offsetParpadeo = 0.0f;
    luces.push_back(luz);
}

void Escenario::AreaCajas() {
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
    suelo.tieneColision = true;
    suelo.esPuerta = false;
    suelo.tieneTextura = true;
    suelo.texturaID = texturaSuelo;
    objetos.push_back(suelo);

    ObjetoFisico techo = suelo;
    techo.posicion.y = techoY;
    techo.color = colPared;
    techo.tieneColision = true;
    techo.texturaID = texturatecho;
    objetos.push_back(techo);

    ObjetoFisico pDer;
    pDer.posicion = glm::vec3(xCentro - anchoSala/2.0f, 0.0f, zCentro);
    pDer.escala = glm::vec3(0.2f, alto, largoSala);
    pDer.color = colPared;
    pDer.tieneColision = true;
    pDer.esPuerta = false;
    pDer.tieneTextura = true;
    pDer.texturaID = texturaPared2;
    objetos.push_back(pDer);

    float pAncho = 2.2f;
    float pAlto = 5.0f;
    float largoParedMitad = (largoSala - pAncho) / 2.0f;

    ObjetoFisico pIzq1;
    pIzq1.posicion = glm::vec3(xCentro + anchoSala/2.0f, 0.0f, zInicio + largoParedMitad/2.0f);
    pIzq1.escala = glm::vec3(0.2f, alto, largoParedMitad);
    pIzq1.color = colPared;
    pIzq1.tieneColision = true;
    pIzq1.esPuerta = false;
    pIzq1.tieneTextura = true;
    pIzq1.texturaID = texturaPared2;
    objetos.push_back(pIzq1);

    ObjetoFisico pIzq2;
    pIzq2.posicion = glm::vec3(xCentro + anchoSala/2.0f, 0.0f, zCentro + pAncho/2.0f + largoParedMitad/2.0f);
    pIzq2.escala = glm::vec3(0.2f, alto, largoParedMitad);
    pIzq2.color = colPared;
    pIzq2.tieneColision = true;
    pIzq2.esPuerta = false;
    pIzq2.tieneTextura = true;
    pIzq2.texturaID = texturaPared2;
    objetos.push_back(pIzq2);

    ObjetoFisico pIzqSup;
    pIzqSup.posicion = glm::vec3(xCentro + anchoSala/2.0f, sueloY + pAlto + (alto - pAlto)/2.0f, zCentro);
    pIzqSup.escala = glm::vec3(0.2f, alto - pAlto, pAncho);
    pIzqSup.color = colPared;
    pIzqSup.tieneColision = true;
    pIzqSup.esPuerta = false;
    pIzqSup.tieneTextura = true;
    pIzqSup.texturaID = texturaPared2;
    objetos.push_back(pIzqSup);

    puertasMadera.push_back(Puerta(glm::vec3(xCentro + anchoSala/2.0f + 0.1f, sueloY + 1.5f, zCentro), 2.2f, 7.0f, 0.1f, 90.0f));
    puertasMadera.back().setTextura("Textures/PuertaMadera.png");
    puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");

    float anchoParedMitad = (anchoSala - pAncho) / 2.0f;

    ObjetoFisico pFrente1;
    pFrente1.posicion = glm::vec3(xCentro - pAncho/2.0f - anchoParedMitad/2.0f, 0.0f, zInicio + largoSala);
    pFrente1.escala = glm::vec3(anchoParedMitad, alto, 0.2f);
    pFrente1.color = colPared;
    pFrente1.tieneColision = true;
    pFrente1.esPuerta = false;
    pFrente1.tieneTextura = true;
    pFrente1.texturaID = texturaPared2;
    objetos.push_back(pFrente1);

    ObjetoFisico pFrente2;
    pFrente2.posicion = glm::vec3(xCentro + pAncho/2.0f + anchoParedMitad/2.0f, 0.0f, zInicio + largoSala);
    pFrente2.escala = glm::vec3(anchoParedMitad, alto, 0.2f);
    pFrente2.color = colPared;
    pFrente2.tieneColision = true;
    pFrente2.esPuerta = false;
    pFrente2.tieneTextura = true;
    pFrente2.texturaID = texturaPared2;
    objetos.push_back(pFrente2);

    ObjetoFisico pFrenteSup;
    pFrenteSup.posicion = glm::vec3(xCentro, sueloY + pAlto + (alto - pAlto)/2.0f, zInicio + largoSala);
    pFrenteSup.escala = glm::vec3(pAncho, alto - pAlto, 0.2f);
    pFrenteSup.color = colPared;
    pFrenteSup.tieneColision = true;
    pFrenteSup.esPuerta = false;
    pFrenteSup.tieneTextura = true;
    pFrenteSup.texturaID = texturaPared2;
    objetos.push_back(pFrenteSup);

    puertasMadera.push_back(Puerta(glm::vec3(xCentro, sueloY + 1.5f, zInicio + largoSala - 0.1f), 2.2f, 7.0f, 0.1f, 0.0f));
    puertasMadera.back().setTextura("Textures/PuertaMadera.png");
    puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");

    float anchoPasillo = 3.0f;
    float anchoParedTraseraMitad = (anchoSala - anchoPasillo) / 2.0f;

    ObjetoFisico pAtras1;
    pAtras1.posicion = glm::vec3(xCentro - anchoPasillo/2.0f - anchoParedTraseraMitad/2.0f, 0.0f, zInicio);
    pAtras1.escala = glm::vec3(anchoParedTraseraMitad, alto, 0.2f);
    pAtras1.color = colPared;
    pAtras1.tieneColision = true;
    pAtras1.esPuerta = false;
    pAtras1.tieneTextura = true;
    pAtras1.texturaID = texturaPared2;
    objetos.push_back(pAtras1);

    ObjetoFisico pAtras2;
    pAtras2.posicion = glm::vec3(xCentro + anchoPasillo/2.0f + anchoParedTraseraMitad/2.0f, 0.0f, zInicio);
    pAtras2.escala = glm::vec3(anchoParedTraseraMitad, alto, 0.2f);
    pAtras2.color = colPared;
    pAtras2.tieneColision = true;
    pAtras2.esPuerta = false;
    pAtras2.tieneTextura = true;
    pAtras2.texturaID = texturaPared2;
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
            caja.tieneTextura = true;
            int texturaIndex = (i + j) % 3;
            if (texturaIndex == 0) {
                caja.texturaID = texturaCaja1;
            } else if (texturaIndex == 1) {
                caja.texturaID = texturaCaja2;
            } else {
                caja.texturaID = texturaCaja3;
            }
            objetos.push_back(caja);
        }
    }

    float luzOffsetX = anchoSala / 3.5f;
    float luzOffsetZ = largoSala / 3.5f;
    float alturaLampara = techoY - 0.05f;

    LucesRectangulares(glm::vec3(xCentro - luzOffsetX, alturaLampara, zCentro - luzOffsetZ));
    LucesRectangulares(glm::vec3(xCentro + luzOffsetX, alturaLampara, zCentro - luzOffsetZ));
    LucesRectangulares(glm::vec3(xCentro - luzOffsetX, alturaLampara, zCentro + luzOffsetZ));
    LucesRectangulares(glm::vec3(xCentro + luzOffsetX, alturaLampara, zCentro + luzOffsetZ));
}

void Escenario::PasillosAreaCajas() {
    float sueloY = -alto / 2.0f;
    float techoY = alto / 2.0f;
    glm::vec3 colParedPasillo = glm::vec3(0.22f, 0.20f, 0.24f);
    glm::vec3 colSueloPasillo = glm::vec3(0.1f, 0.1f, 0.1f);
    float anchoPasillo = 3.0f;
    float largoPasillo = 10.0f;
    float xNuevaAreaCentro = -7.5f;
    float anchoNuevaArea = 14.0f;
    float largoNuevaArea = 14.0f;
    float zNuevaAreaInicio = (profundo / 2.0f) + 15.0f;
    float pAnchoD = 2.2f;
    float pAltoD = 5.0f;

    float zFinalNuevaArea = zNuevaAreaInicio + largoNuevaArea;

    ObjetoFisico sueloExt1;
    sueloExt1.posicion = glm::vec3(xNuevaAreaCentro, sueloY, zFinalNuevaArea + largoPasillo / 2.0f);
    sueloExt1.escala = glm::vec3(anchoPasillo, 0.05f, largoPasillo);
    sueloExt1.color = colSueloPasillo;
    sueloExt1.tieneColision = true;
    sueloExt1.esPuerta = false;
    sueloExt1.tieneTextura = true;
    sueloExt1.texturaID = texturaSuelo;
    objetos.push_back(sueloExt1);

    ObjetoFisico techoExt1 = sueloExt1;
    techoExt1.posicion.y = techoY;
    techoExt1.color = colParedPasillo;
    techoExt1.tieneTextura = true;
    techoExt1.texturaID = texturatecho;
    objetos.push_back(techoExt1);

    ObjetoFisico pDerExt1;
    pDerExt1.posicion = glm::vec3(xNuevaAreaCentro + anchoPasillo/2.0f, 0.0f, zFinalNuevaArea + largoPasillo / 2.0f);
    pDerExt1.escala = glm::vec3(0.1f, alto, largoPasillo);
    pDerExt1.color = colParedPasillo;
    pDerExt1.tieneColision = true;
    pDerExt1.esPuerta = false;
    pDerExt1.tieneTextura = true;
    pDerExt1.texturaID = texturaPared2;
    objetos.push_back(pDerExt1);

    ObjetoFisico pIzqExt1 = pDerExt1;
    pIzqExt1.posicion.x = xNuevaAreaCentro - anchoPasillo/2.0f;
    objetos.push_back(pIzqExt1);

    float xAnchoParedMitad = (anchoPasillo - pAnchoD) / 2.0f;

    ObjetoFisico pFin1Der;
    pFin1Der.posicion = glm::vec3(xNuevaAreaCentro + pAnchoD/2.0f + xAnchoParedMitad/2.0f, 0.0f, zFinalNuevaArea + largoPasillo);
    pFin1Der.escala = glm::vec3(xAnchoParedMitad, alto, 0.1f);
    pFin1Der.color = colParedPasillo;
    pFin1Der.tieneColision = true;
    pFin1Der.esPuerta = false;
    pFin1Der.tieneTextura = true;
    pFin1Der.texturaID = texturaPared2;
    objetos.push_back(pFin1Der);

    ObjetoFisico pFin1Izq = pFin1Der;
    pFin1Izq.posicion.x = xNuevaAreaCentro - pAnchoD/2.0f - xAnchoParedMitad/2.0f;
    objetos.push_back(pFin1Izq);

    ObjetoFisico pFin1Sup;
    pFin1Sup.posicion = glm::vec3(xNuevaAreaCentro, sueloY + pAltoD + (alto - pAltoD)/2.0f, zFinalNuevaArea + largoPasillo);
    pFin1Sup.escala = glm::vec3(pAnchoD, alto - pAltoD, 0.1f);
    pFin1Sup.color = colParedPasillo;
    pFin1Sup.tieneColision = true;
    pFin1Sup.esPuerta = false;
    pFin1Sup.tieneTextura = true;
    pFin1Sup.texturaID = texturaPared2;
    objetos.push_back(pFin1Sup);

    puertasMadera.push_back(Puerta(glm::vec3(xNuevaAreaCentro, sueloY + 1.5f, zFinalNuevaArea + largoPasillo - 0.1f), 2.2f, 7.0f, 0.1f, 0.0f));
    puertasMadera.back().setTextura("Textures/PuertaMadera.png");
    puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");

    float xNuevaAreaDer = xNuevaAreaCentro + anchoNuevaArea/2.0f;
    float zCentroRamificacion = zNuevaAreaInicio + largoNuevaArea / 2.0f;

    ObjetoFisico sueloExt2;
    sueloExt2.posicion = glm::vec3(xNuevaAreaDer + largoPasillo / 2.0f, sueloY, zCentroRamificacion);
    sueloExt2.escala = glm::vec3(largoPasillo, 0.05f, anchoPasillo);
    sueloExt2.color = colSueloPasillo;
    sueloExt2.tieneColision = true;
    sueloExt2.esPuerta = false;
    sueloExt2.tieneTextura = true;
    sueloExt2.texturaID = texturaSuelo;
    objetos.push_back(sueloExt2);

    ObjetoFisico techoExt2 = sueloExt2;
    techoExt2.posicion.y = techoY;
    techoExt2.color = colParedPasillo;
    objetos.push_back(techoExt2);

    ObjetoFisico pFrenteExt2;
    pFrenteExt2.posicion = glm::vec3(xNuevaAreaDer + largoPasillo / 2.0f, 0.0f, zCentroRamificacion + anchoPasillo/2.0f);
    pFrenteExt2.escala = glm::vec3(largoPasillo, alto, 0.1f);
    pFrenteExt2.color = colParedPasillo;
    pFrenteExt2.tieneColision = true;
    pFrenteExt2.esPuerta = false;
    pFrenteExt2.tieneTextura = true;
    pFrenteExt2.texturaID = texturaPared2;
    objetos.push_back(pFrenteExt2);

    ObjetoFisico pAtrasExt2 = pFrenteExt2;
    pAtrasExt2.posicion.z = zCentroRamificacion - anchoPasillo/2.0f;
    objetos.push_back(pAtrasExt2);

    float zAnchoParedMitad = (anchoPasillo - pAnchoD) / 2.0f;

    ObjetoFisico pFin2Der;
    pFin2Der.posicion = glm::vec3(xNuevaAreaDer + largoPasillo, 0.0f, zCentroRamificacion + pAnchoD/2.0f + zAnchoParedMitad/2.0f);
    pFin2Der.escala = glm::vec3(0.1f, alto, zAnchoParedMitad);
    pFin2Der.color = colParedPasillo;
    pFin2Der.tieneColision = true;
    pFin2Der.esPuerta = false;
    pFin2Der.tieneTextura = true;
    pFin2Der.texturaID = texturaPared2;
    objetos.push_back(pFin2Der);

    ObjetoFisico pFin2Izq = pFin2Der;
    pFin2Izq.posicion.z = zCentroRamificacion - pAnchoD/2.0f - zAnchoParedMitad/2.0f;
    objetos.push_back(pFin2Izq);

    ObjetoFisico pFin2Sup;
    pFin2Sup.posicion = glm::vec3(xNuevaAreaDer + largoPasillo, sueloY + pAltoD + (alto - pAltoD)/2.0f, zCentroRamificacion);
    pFin2Sup.escala = glm::vec3(0.1f, alto - pAltoD, pAnchoD);
    pFin2Sup.color = colParedPasillo;
    pFin2Sup.tieneColision = true;
    pFin2Sup.esPuerta = false;
    pFin2Sup.tieneTextura = true;
    pFin2Sup.texturaID = texturaPared2;
    objetos.push_back(pFin2Sup);

    puertasMadera.push_back(Puerta(glm::vec3(xNuevaAreaDer + largoPasillo - 0.1f, sueloY + 1.5f, zCentroRamificacion), 2.2f, 7.0f, 0.1f, 90.0f));
    puertasMadera.back().setTextura("Textures/PuertaMadera.png");
    puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");

    float alturaLampara = techoY - 0.05f;

    LucesRectangulares(glm::vec3(xNuevaAreaCentro, alturaLampara, zFinalNuevaArea + (largoPasillo / 3.0f)));
    LucesRectangulares(glm::vec3(xNuevaAreaCentro, alturaLampara, zFinalNuevaArea + (largoPasillo * 2.0f / 3.0f)));

    float separacionX = largoPasillo / 3.0f;
    for(int i = 1; i <= 2; i++) {
        glm::vec3 posLuz2 = glm::vec3(xNuevaAreaDer + (separacionX * i), alturaLampara, zCentroRamificacion);

        ObjetoFisico panelLuz2;
        panelLuz2.posicion = posLuz2;
        panelLuz2.escala = glm::vec3(1.0f, 0.1f, 2.5f);
        panelLuz2.color = glm::vec3(0.9f, 0.9f, 0.95f);
        panelLuz2.tieneColision = false;
        panelLuz2.esPuerta = false;
        objetos.push_back(panelLuz2);

        LuzPuntual luz2;
        luz2.posicion = posLuz2 - glm::vec3(0.0f, 0.1f, 0.0f);
        luz2.intensidad = 1.2f;
        luz2.color = glm::vec3(0.7f, 0.7f, 0.8f);
        luz2.parpadea = false;
        luz2.direction = glm::vec3(0.0f, -1.0f, 0.0f);
        luz2.cutOff = glm::cos(glm::radians(45.0f));
        luz2.outerCutOff = glm::cos(glm::radians(60.0f));
        luz2.tiempoParpadeo = 0.0f;
        luz2.offsetParpadeo = 0.0f;
        luz2.diffuseStrength = 0.4f;
        luces.push_back(luz2);
    }
}

void Escenario::SalaJefe() {
    float sueloY = -alto / 2.0f;
    float techoY = alto / 2.0f;
    glm::vec3 colPared = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 colSuelo = glm::vec3(0.15f, 0.15f, 0.15f);
    float anchoArea = 14.0f;
    float largoArea = 14.0f;
    float xCentro = -7.5f;
    float zCentro = 54.0f + (largoArea / 2.0f);

    ObjetoFisico suelo;
    suelo.posicion = glm::vec3(xCentro, sueloY, zCentro);
    suelo.escala = glm::vec3(anchoArea, 0.05f, largoArea);
    suelo.color = colSuelo;
    suelo.tieneColision = true;
    suelo.esPuerta = false;
    suelo.tieneTextura = true;
    suelo.texturaID = texturaSueloJefe;
    objetos.push_back(suelo);

    ObjetoFisico techo = suelo;
    techo.posicion.y = techoY;
    techo.color = colPared;
    techo.tieneTextura = true;
    techo.texturaID = texturatecho;
    objetos.push_back(techo);

    ObjetoFisico pIzq;
    pIzq.posicion = glm::vec3(xCentro - anchoArea/2.0f, 0.0f, zCentro);
    pIzq.escala = glm::vec3(0.1f, alto, largoArea);
    pIzq.color = colPared;
    pIzq.tieneColision = true;
    pIzq.esPuerta = false;
    pIzq.tieneTextura = true;
    pIzq.texturaID = texturaPared3;
    objetos.push_back(pIzq);

    ObjetoFisico pDer = pIzq;
    pDer.posicion.x = xCentro + anchoArea/2.0f;
    objetos.push_back(pDer);

    ObjetoFisico pFrente;
    pFrente.posicion = glm::vec3(xCentro, 0.0f, zCentro + largoArea/2.0f);
    pFrente.escala = glm::vec3(anchoArea, alto, 0.1f);
    pFrente.color = colPared;
    pFrente.tieneColision = true;
    pFrente.esPuerta = false;
    pFrente.tieneTextura = true;
    pFrente.texturaID = texturaPared3;
    objetos.push_back(pFrente);

    float anchoParedLateral = (anchoArea - 3.0f) / 2.0f;
    ObjetoFisico pAtrasIzq;
    pAtrasIzq.posicion = glm::vec3(-14.5f + anchoParedLateral/2.0f, 0.0f, 54.0f);
    pAtrasIzq.escala = glm::vec3(anchoParedLateral, alto, 0.1f);
    pAtrasIzq.color = colPared;
    pAtrasIzq.tieneColision = true;
    pAtrasIzq.esPuerta = false;
    pAtrasIzq.tieneTextura = true;
    pAtrasIzq.texturaID = texturaPared3;
    objetos.push_back(pAtrasIzq);

    ObjetoFisico pAtrasDer = pAtrasIzq;
    pAtrasDer.posicion.x = -0.5f - anchoParedLateral/2.0f;
    objetos.push_back(pAtrasDer);

    unsigned int texturaMan = cargarTextura("textures/man.png");

    ObjetoFisico objetoMan;
    objetoMan.posicion = glm::vec3(-7.5f, 2.0f, 67.5f);
    objetoMan.escala = glm::vec3(4.0f, 5.0f, 0.05f);
    objetoMan.rotacionY = 45.0f;
    objetoMan.tieneTextura = true;
    objetoMan.texturaID = texturaMan;
    objetoMan.tieneColision = true;
    objetoMan.esPuerta = false;
    objetos.push_back(objetoMan);

    std::cout << "Objeto con textura man.png agregado en AreaFinal1" << std::endl;

    static Model* modeloMesa = nullptr;
    if (!modeloMesa) {
        modeloMesa = new Model("models/MesaTrabajo/MesaTrabajo.obj");
        modeloMesa->position = glm::vec3(-7.5f, -5.0f, 64.0f);
        modeloMesa->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        modeloMesa->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa posicionada" << std::endl;
    }
    modelosExtra.push_back(modeloMesa);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    ObjetoFisico hitbox;

    hitbox.posicion = glm::vec3(-7.5f, -5.0f, 64.0f);
    hitbox.escala   = glm::vec3(5.5f, 10.0f, 2.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* modeloAbanico = nullptr;
    if (!modeloAbanico) {
        modeloAbanico = new Model("models/Abanico/Abanico.obj");
        modeloAbanico->position = glm::vec3(-7.5f, -5.0f, 64.0f);
        modeloAbanico->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        modeloAbanico->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Abanico cargado" << std::endl;
    }
    modelosExtra.push_back(modeloAbanico);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* modeloSilla = nullptr;
    if (!modeloSilla) {
        modeloSilla = new Model("models/Silla/Silla.obj");
        modeloSilla->position = glm::vec3(-5.5f, -5.0f, 63.9f);
        modeloSilla->rotation = glm::vec3(0.0f, 40.0f, 0.0f);
        modeloSilla->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Silla cargada" << std::endl;
    }
    modelosExtra.push_back(modeloSilla);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* modeloCajon = nullptr;
    if (!modeloCajon) {
        modeloCajon = new Model("models/Cajon/Cajon.obj");
        modeloCajon->position = glm::vec3(-12.0f, -4.5f, 66.0f);
        modeloCajon->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        modeloCajon->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Cajón cargado" << std::endl;
    }
    modelosExtra.push_back(modeloCajon);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(-11.5f, -5.0f, 67.5f);
    hitbox.escala   = glm::vec3(3.5f, 10.0f, 3.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* modeloPantalla = nullptr;
    if (!modeloPantalla) {
        modeloPantalla = new Model("models/Pantalla/Pantalla.obj");
        modeloPantalla->position = glm::vec3(-7.5f, -5.0f, 64.0f);
        modeloPantalla->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        modeloPantalla->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Pantalla cargada" << std::endl;
    }
    modelosExtra.push_back(modeloPantalla);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* Taza = nullptr;
    if (!Taza) {
        Taza = new Model("models/pencilcup/Tasa.obj");
        Taza->position = glm::vec3(-8.2f, -5.0f, 63.7f);
        Taza->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        Taza->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Pantalla cargada" << std::endl;
    }
    modelosExtra.push_back(Taza);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* maceta = nullptr;
    if (!maceta) {
        maceta = new Model("models/Maceta/maceta.obj");
        maceta->position = glm::vec3(-3.0f, -5.0f, 67.0f);
        maceta->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        maceta->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Cajón cargado" << std::endl;
    }
    modelosExtra.push_back(maceta);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(-3.0f, -5.0f, 67.0f);
    hitbox.escala   = glm::vec3(1.5f, 10.0f, 1.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* papelera = nullptr;
    if (!papelera) {
        papelera = new Model("models/trashcan/papelera.obj");
        papelera->position = glm::vec3(-4.0f, -5.0f, 64.5f);
        papelera->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        papelera->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Cajón cargado" << std::endl;
    }
    modelosExtra.push_back(papelera);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(-4.0f, -5.0f, 64.5f);
    hitbox.escala   = glm::vec3(1.0f, 10.0f, 1.0f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    // Botón de salida en la pared de la oficina del jefe
    agregarBoton(glm::vec3(-3.0f, -2.5f, 67.9f), glm::vec3(0.2f, 0.2f, 0.1f), glm::vec3(0.2f, 0.8f, 0.2f), 4, 2.0f);

    float alturaLampara = techoY - 0.05f;
    for(int i = -1; i <= 1; i += 2) {
        glm::vec3 posLuz = glm::vec3(xCentro, alturaLampara, zCentro + (i * 3.0f));

        ObjetoFisico panelLuz;
        panelLuz.posicion = posLuz;
        panelLuz.escala = glm::vec3(4.0f, 0.1f, 1.5f);
        panelLuz.color = glm::vec3(0.9f, 0.9f, 0.95f);
        panelLuz.tieneColision = false;
        panelLuz.esPuerta = false;
        objetos.push_back(panelLuz);

        LuzPuntual luz;
        luz.posicion = posLuz - glm::vec3(0.0f, 0.1f, 0.0f);
        luz.intensidad = 1.4f;
        luz.color = glm::vec3(0.8f, 0.8f, 0.9f);
        luz.parpadea = false;
        luz.direction = glm::vec3(0.0f, -1.0f, 0.0f);
        luz.cutOff = glm::cos(glm::radians(50.0f));
        luz.outerCutOff = glm::cos(glm::radians(70.0f));
        luz.tiempoParpadeo = 0.0f;
        luz.offsetParpadeo = 0.0f;
        luz.diffuseStrength = 1.0f;
        luces.push_back(luz);
    }
}

void Escenario::AreaSeguridad() {
    float sueloY = -alto / 2.0f;
    float techoY = alto / 2.0f;
    glm::vec3 colPared = glm::vec3(0.18f, 0.2f, 0.18f);
    glm::vec3 colSuelo = glm::vec3(0.15f, 0.15f, 0.15f);
    float anchoArea = 18.0f;
    float largoArea = 18.0f;
    float xCentro = 9.5f + (anchoArea / 2.0f);
    float zCentro = 37.0f;
    float pAncho = 2.2f;
    float pAlto = 5.0f;

    ObjetoFisico suelo;
    suelo.posicion = glm::vec3(xCentro, sueloY, zCentro);
    suelo.escala = glm::vec3(anchoArea, 0.05f, largoArea);
    suelo.color = colSuelo;
    suelo.tieneColision = true;
    suelo.esPuerta = false;
    suelo.tieneTextura = true;
    suelo.texturaID = texturaSueloAlfombra;
    objetos.push_back(suelo);

    ObjetoFisico techo = suelo;
    techo.posicion.y = techoY;
    techo.color = colPared;
    techo.tieneTextura= true;
    techo.texturaID = texturatecho;
    objetos.push_back(techo);

    ObjetoFisico hitbox;

    auto crearParedConPuerta = [&](glm::vec3 pos, bool orientacionZ, float rotPuerta) {
        float paredLargo = orientacionZ ? largoArea : anchoArea;
        float anchoMuro = (paredLargo - pAncho) / 2.0f;

        ObjetoFisico p1, p2, pSup;
        p1.color = colPared;
        p1.tieneColision = true;
        p1.esPuerta = false;
        p1.tieneTextura = true;
        p1.texturaID = texturaPared3;
        p2 = p1;
        pSup = p1;

        if (orientacionZ) {
            p1.posicion = pos + glm::vec3(0.0f, 0.0f, pAncho/2.0f + anchoMuro/2.0f);
            p1.escala = glm::vec3(0.1f, alto, anchoMuro);

            p2.posicion = pos - glm::vec3(0.0f, 0.0f, pAncho/2.0f + anchoMuro/2.0f);
            p2.escala = glm::vec3(0.1f, alto, anchoMuro);

            pSup.posicion = pos + glm::vec3(0.0f, sueloY + pAlto + (alto - pAlto)/2.0f, 0.0f);
            pSup.escala = glm::vec3(0.1f, alto - pAlto, pAncho);
        } else {
            p1.posicion = pos + glm::vec3(pAncho/2.0f + anchoMuro/2.0f, 0.0f, 0.0f);
            p1.escala = glm::vec3(anchoMuro, alto, 0.1f);

            p2.posicion = pos - glm::vec3(pAncho/2.0f + anchoMuro/2.0f, 0.0f, 0.0f);
            p2.escala = glm::vec3(anchoMuro, alto, 0.1f);

            pSup.posicion = pos + glm::vec3(0.0f, sueloY + pAlto + (alto - pAlto)/2.0f, 0.0f);
            pSup.escala = glm::vec3(pAncho, alto - pAlto, 0.1f);
        }

        objetos.push_back(p1);
        objetos.push_back(p2);
        objetos.push_back(pSup);
        puertasMadera.push_back(Puerta(glm::vec3(pos.x, sueloY + 1.5f, pos.z), 2.2f, 7.0f, 0.1f, rotPuerta));
        puertasMadera.back().setTextura("Textures/PuertaMadera.png");
        puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");
    };

    crearParedConPuerta(glm::vec3(xCentro + anchoArea/2.0f, 0.0f, zCentro), true, 90.0f);
    crearParedConPuerta(glm::vec3(xCentro, 0.0f, zCentro + largoArea/2.0f), false, 0.0f);
    crearParedConPuerta(glm::vec3(xCentro, 0.0f, zCentro - largoArea/2.0f), false, 180.0f);

    float anchoParedLateral = (largoArea - 3.0f) / 2.0f;
    ObjetoFisico pIzqArriba;
    pIzqArriba.posicion = glm::vec3(9.5f, 0.0f, 38.5f + anchoParedLateral/2.0f);
    pIzqArriba.escala = glm::vec3(0.1f, alto, anchoParedLateral);
    pIzqArriba.color = colPared;
    pIzqArriba.tieneColision = true;
    pIzqArriba.esPuerta = false;
    pIzqArriba.tieneTextura = true;
    pIzqArriba.texturaID = texturaPared3;
    objetos.push_back(pIzqArriba);

    ObjetoFisico pIzqAbajo = pIzqArriba;
    pIzqAbajo.posicion.z = 35.5f - anchoParedLateral/2.0f;
    objetos.push_back(pIzqAbajo);

    static Model* modeloReloj = nullptr;
    if (!modeloReloj) {
        modeloReloj = new Model("models/Relog/Relog.obj");
        modeloReloj->position = glm::vec3(xCentro - 3.5f, sueloY + 5.5f, zCentro + 8.8f);
        modeloReloj->rotation = glm::vec3(0.0f, 180.0f, 0.0f);
        modeloReloj->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Reloj agregado" << std::endl;
    }
    modelosExtra.push_back(modeloReloj);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* modeloMesaBase = nullptr;
    static Model* modeloSillaBase = nullptr;
    static Model* modeloPantallaBase = nullptr;

    if (!modeloMesaBase) {
        modeloMesaBase = new Model("models/MesaTrabajo/MesaTrabajo.obj");
        modeloMesaBase->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Modelo de Mesa base cargado" << std::endl;
    }
    modelosExtra.push_back(modeloMesaBase);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    if (!modeloSillaBase) {
        modeloSillaBase = new Model("models/Silla/Silla.obj");
        modeloSillaBase->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Modelo de Silla base cargado" << std::endl;
    }

    if (!modeloPantallaBase) {
        modeloPantallaBase = new Model("models/Pantalla/Pantalla.obj");
        modeloPantallaBase->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Modelo de Pantalla base cargado" << std::endl;
    }

    //Primera seccion modelos

    static Model* modeloMesa = nullptr;
    if (!modeloMesa) {
        modeloMesa = new Model("models/MesaTrabajo/MesaTrabajo.obj");
        modeloMesa->position = glm::vec3(11.0f, -4.7f, 42.5f);
        modeloMesa->rotation = glm::vec3(0.0f, 180.0f, 0.0f);
        modeloMesa->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 1 agregada" << std::endl;
    }
    modelosExtra.push_back(modeloMesa);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(11.0f, -5.0f, 42.5f);
    hitbox.escala   = glm::vec3(2.8f, 10.0f, 6.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* Teclado1 = nullptr;
    if (!Teclado1) {
        Teclado1 = new Model("models/keyboard/teclado.obj");
        Teclado1->position = glm::vec3(11.6f, -4.7f, 41.5f);
        Teclado1->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        Teclado1->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 1 agregada" << std::endl;
    }
    modelosExtra.push_back(Teclado1);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* Papeles1 = nullptr;
    if (!Papeles1) {
        Papeles1 = new Model("models/Papeles/papel1.obj");
        Papeles1->position = glm::vec3(9.6f, -6.7f, 44.5f);
        Papeles1->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        Papeles1->scale = glm::vec3(2.0f, 2.0f, 2.0f);
        std::cout << "Mesa 1 agregada" << std::endl;
    }
    modelosExtra.push_back(Papeles1);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* modeloSilla = nullptr;
    if (!modeloSilla) {
        modeloSilla = new Model("models/Silla/Silla.obj");
        modeloSilla->position = glm::vec3(12.5f, -4.7f, 39.5f);
        modeloSilla->rotation = glm::vec3(0.0f, 95.0f, 0.0f);
        modeloSilla->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Silla 1 agregada" << std::endl;
    }
    modelosExtra.push_back(modeloSilla);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(13.5f, -5.0f, 42.5f);
    hitbox.escala   = glm::vec3(2.5f, 10.0f, 2.0f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* modeloPantalla = nullptr;
    if (!modeloPantalla) {
        modeloPantalla = new Model("models/Pantalla/Pantalla.obj");
        modeloPantalla->position = glm::vec3(11.5f, -4.6f, 43.0f);
        modeloPantalla->rotation = glm::vec3(0.0f, -165.5f, 0.0f);
        modeloPantalla->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Pantalla 1 agregada" << std::endl;
    }
    modelosExtra.push_back(modeloPantalla);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    // Segunda seccion modelos
    static Model* modeloMesa2 = nullptr;
    if (!modeloMesa2) {
        modeloMesa2 = new Model("models/MesaTrabajo/MesaTrabajo.obj");
        modeloMesa2->position = glm::vec3(11.0f, -4.7f, 32.0f);
        modeloMesa2->rotation = glm::vec3(0.0f, 180.0f, 0.0f);
        modeloMesa2->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 2 agregada" << std::endl;
    }
    modelosExtra.push_back(modeloMesa2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    modelosExtra.push_back(modeloMesa2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(11.0f, -5.0f, 32.0f);
    hitbox.escala   = glm::vec3(2.8f, 10.0f, 6.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* Papeles2 = nullptr;
    if (!Papeles2) {
        Papeles2 = new Model("models/Papeles/papel1.obj");
        Papeles2->position = glm::vec3(9.6f, -6.7f, 30.0f);
        Papeles2->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        Papeles2->scale = glm::vec3(2.0f, 2.0f, 2.0f);
        std::cout << "Mesa 1 agregada" << std::endl;
    }
    modelosExtra.push_back(Papeles2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* Papeles3 = nullptr;
    if (!Papeles3) {
        Papeles3 = new Model("models/Papeles/papel1.obj");
        Papeles3->position = glm::vec3(9.6f, -6.7f, 34.0f);
        Papeles3->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        Papeles3->scale = glm::vec3(2.0f, 2.0f, 2.0f);
        std::cout << "Mesa 1 agregada" << std::endl;
    }
    modelosExtra.push_back(Papeles3);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* Teclado2 = nullptr;
    if (!Teclado2) {
        Teclado2 = new Model("models/keyboard/Teclado.obj");
        Teclado2->position = glm::vec3(11.6f, -4.7f, 31.0f);
        Teclado2->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        Teclado2->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 2 agregada" << std::endl;
    }
    modelosExtra.push_back(Teclado2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* modeloSilla2 = nullptr;
    if (!modeloSilla2) {
        modeloSilla2 = new Model("models/Silla/Silla.obj");
        modeloSilla2->position = glm::vec3(12.5f, -4.7f, 29.0f);
        modeloSilla2->rotation = glm::vec3(0.0f, 95.0f, 0.0f);
        modeloSilla2->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Silla 2 agregada" << std::endl;
    }
    modelosExtra.push_back(modeloSilla2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(13.0f, -5.0f, 32.0f);
    hitbox.escala   = glm::vec3(2.5f, 10.0f, 2.0f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* modeloPantalla2 = nullptr;
    if (!modeloPantalla2) {
        modeloPantalla2 = new Model("models/Pantalla/Pantalla.obj");
        modeloPantalla2->position = glm::vec3(11.5f, -4.6f, 32.5f);
        modeloPantalla2->rotation = glm::vec3(0.0f, -165.5f, 0.0f);
        modeloPantalla2->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Pantalla 2 agregada" << std::endl;
    }
    modelosExtra.push_back(modeloPantalla2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    // Tercera seccion modelos
    static Model* modeloCajon1 = nullptr;
    if (!modeloCajon1) {
        modeloCajon1 = new Model("models/Cajon/Cajon.obj");
        modeloCajon1->position = glm::vec3(25.5f, -4.5f, 29.9f);
        modeloCajon1->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
        modeloCajon1->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Cajón 1 agregado en AreaFinal2" << std::endl;
    }
    modelosExtra.push_back(modeloCajon1);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* modeloCajon2 = nullptr;
    if (!modeloCajon2) {
        modeloCajon2 = new Model("models/Cajon/Cajon.obj");
        modeloCajon2->position = glm::vec3(21.5f, -4.5f, 29.9f);
        modeloCajon2->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
        modeloCajon2->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Cajón 2 agregado en AreaFinal2" << std::endl;
    }
    modelosExtra.push_back(modeloCajon2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(23.5f, -5.0f, 27.9f);
    hitbox.escala   = glm::vec3(6.7f, 10.0f, 6.0f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    // Cuarta seccion modelos
    static Model* modeloMesa3 = nullptr;
    if (!modeloMesa3) {
        modeloMesa3 = new Model("models/MesaTrabajo/MesaTrabajo.obj");
        modeloMesa3->position = glm::vec3(26.3f, -4.7f, 41.5f);
        modeloMesa3->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        modeloMesa3->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 3 agregada" << std::endl;
    }
    modelosExtra.push_back(modeloMesa3);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    modelosExtra.push_back(modeloMesa3);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(26.3f, -5.0f, 41.5f);
    hitbox.escala   = glm::vec3(2.8f, 10.0f, 6.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* Tasa1 = nullptr;
    if (!Tasa1) {
        Tasa1 = new Model("models/pencilcup/Tasa2.obj");
        Tasa1->position = glm::vec3(26.3f, -4.7f, 43.0f);
        Tasa1->rotation = glm::vec3(0.0f, 180.0f, 0.0f);
        Tasa1->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 3 agregada" << std::endl;
    }
    modelosExtra.push_back(Tasa1);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* Papeles4 = nullptr;
    if (!Papeles4) {
        Papeles4 = new Model("models/Papeles/papel1.obj");
        Papeles4->position = glm::vec3(24.5f, -6.7f, 39.5f);
        Papeles4->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        Papeles4->scale = glm::vec3(2.0f, 2.0f, 2.0f);
        std::cout << "Mesa 1 agregada" << std::endl;
    }
    modelosExtra.push_back(Papeles4);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* Papeles5 = nullptr;
    if (!Papeles5) {
        Papeles5 = new Model("models/Papeles/papel3.obj");
        Papeles5->position = glm::vec3(24.5f, -6.8f, 41.5f);
        Papeles5->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        Papeles5->scale = glm::vec3(2.0f, 2.0f, 2.0f);
        std::cout << "Mesa 1 agregada" << std::endl;
    }
    modelosExtra.push_back(Papeles5);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* Basurero1 = nullptr;
    if (!Basurero1) {
        Basurero1 = new Model("models/trashcan/Papelera.obj");
        Basurero1->position = glm::vec3(24.3f, -4.7f, 39.0f);
        Basurero1->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        Basurero1->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 3 agregada" << std::endl;
    }
    modelosExtra.push_back(Basurero1);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* pizarra = nullptr;
    if (!pizarra) {
        pizarra = new Model("models/Whiteboard/WhiteBoard.obj");
        pizarra->position = glm::vec3(24.5f, -4.7f, 45.0f);
        pizarra->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        pizarra->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "pizarra agregada" << std::endl;
    }
    modelosExtra.push_back(pizarra);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    hitbox.posicion = glm::vec3(23.3f, -5.0f, 45.0f);
    hitbox.escala   = glm::vec3(4.5f, 10.0f, 1.2f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* modeloSilla3 = nullptr;
    if (!modeloSilla3) {
        modeloSilla3 = new Model("models/Silla/Silla.obj");
        modeloSilla3->position = glm::vec3(26.2f, -4.7f, 43.5f);
        modeloSilla3->rotation = glm::vec3(0.0f, -60.0f, 0.0f);
        modeloSilla3->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Silla 1 agregada" << std::endl;
    }
    modelosExtra.push_back(modeloSilla3);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    hitbox.posicion = glm::vec3(24.5f, -5.0f, 41.0f);
    hitbox.escala   = glm::vec3(2.5f, 10.0f, 2.0f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    float alturaLampara = techoY - 0.05f;
    float offset = 4.0f;

    for(int x = -1; x <= 1; x += 2) {
        for(int z = -1; z <= 1; z += 2) {
            glm::vec3 posLuz = glm::vec3(xCentro + (x * offset), alturaLampara, zCentro + (z * offset));

            ObjetoFisico panelLuz;
            panelLuz.posicion = posLuz;
            panelLuz.escala = glm::vec3(2.5f, 0.1f, 2.5f);
            panelLuz.color = glm::vec3(0.9f, 0.9f, 0.95f);
            panelLuz.tieneColision = false;
            panelLuz.esPuerta = false;
            objetos.push_back(panelLuz);

            LuzPuntual luz;
            luz.posicion = posLuz - glm::vec3(0.0f, 0.1f, 0.0f);
            luz.intensidad = 1.2f;
            luz.color = glm::vec3(0.8f, 0.8f, 0.9f);
            luz.parpadea = false;
            luz.direction = glm::vec3(0.0f, -1.0f, 0.0f);
            luz.cutOff = glm::cos(glm::radians(45.0f));
            luz.outerCutOff = glm::cos(glm::radians(65.0f));
            luz.tiempoParpadeo = 0.0f;
            luz.offsetParpadeo = 0.0f;
            luz.diffuseStrength = 0.4f;
            luces.push_back(luz);
        }
    }

    std::cout << "AreaFinal2: 3 mesas, 2 sillas, 2 pantallas, 2 cajones y reloj agregados" << std::endl;
}

void Escenario::PasillosAreaSeguridad() {
    float sueloY = -alto / 2.0f;
    float techoY = alto / 2.0f;
    float pAlto = 5.0f;
    glm::vec3 colPared = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 colSuelo = glm::vec3(0.15f, 0.15f, 0.15f);
    float alturaLampara = techoY - 0.05f;

    float xCentroF = 32.5f;
    float zCentroF = 37.0f;

    ObjetoFisico sueloF;
    sueloF.posicion = glm::vec3(xCentroF, sueloY, zCentroF);
    sueloF.escala = glm::vec3(10.0f, 0.05f, 3.0f);
    sueloF.color = colSuelo;
    sueloF.tieneColision = true;
    sueloF.esPuerta = false;
    sueloF.tieneTextura = true;
    sueloF.texturaID = texturaSuelo;
    objetos.push_back(sueloF);

    ObjetoFisico techoF = sueloF;
    techoF.posicion.y = techoY;
    techoF.color = colPared;
    techoF.tieneTextura = true;
    techoF.texturaID = texturatecho;
    objetos.push_back(techoF);

    ObjetoFisico pFrenteF;
    pFrenteF.posicion = glm::vec3(37.5f, 0.0f, zCentroF);
    pFrenteF.escala = glm::vec3(0.1f, alto, 3.0f);
    pFrenteF.color = colPared;
    pFrenteF.tieneColision = true;
    pFrenteF.esPuerta = false;
    pFrenteF.tieneTextura = true;
    pFrenteF.texturaID = texturaPared2;
    objetos.push_back(pFrenteF);

    ObjetoFisico pDerF;
    pDerF.posicion = glm::vec3(xCentroF, 0.0f, 38.5f);
    pDerF.escala = glm::vec3(10.0f, alto, 0.1f);
    pDerF.color = colPared;
    pDerF.tieneColision = true;
    pDerF.esPuerta = false;
    pDerF.tieneTextura = true;
    pDerF.texturaID = texturaPared2;
    objetos.push_back(pDerF);

    ObjetoFisico pIzqF1;
    pIzqF1.posicion = glm::vec3(31.2f, 0.0f, 35.5f);
    pIzqF1.escala = glm::vec3(7.4f, alto, 0.1f);
    pIzqF1.color = colPared;
    pIzqF1.tieneColision = true;
    pIzqF1.esPuerta = false;
    pIzqF1.tieneTextura = true;
    pIzqF1.texturaID = texturaPared2;
    objetos.push_back(pIzqF1);

    ObjetoFisico pIzqF2;
    pIzqF2.posicion = glm::vec3(37.3f, 0.0f, 35.5f);
    pIzqF2.escala = glm::vec3(0.4f, alto, 0.1f);
    pIzqF2.color = colPared;
    pIzqF2.tieneColision = true;
    pIzqF2.esPuerta = false;
    pIzqF2.tieneTextura = true;
    pIzqF2.texturaID = texturaPared2;
    objetos.push_back(pIzqF2);

    ObjetoFisico pIzqSupF;
    pIzqSupF.posicion = glm::vec3(36.0f, sueloY + pAlto + (alto - pAlto)/2.0f, 35.5f);
    pIzqSupF.escala = glm::vec3(2.2f, alto - pAlto, 0.1f);
    pIzqSupF.color = colPared;
    pIzqSupF.tieneColision = true;
    pIzqSupF.esPuerta = false;
    pIzqSupF.tieneTextura = true;
    pIzqSupF.texturaID = texturaPared2;
    objetos.push_back(pIzqSupF);

    puertasMadera.push_back(Puerta(glm::vec3(36.0f, sueloY + 1.5f, 35.5f), 2.2f, 7.0f, 0.1f, 0.0f));
    puertasMadera.back().setTextura("Textures/PuertaMadera.png");
    puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");

    static Model* modeloPalanca = nullptr;
    if (!modeloPalanca) {
        modeloPalanca = new Model("models/crowbar/lever.obj");
        modeloPalanca->position = glm::vec3(37.5f, sueloY + 3.0f, 37.0f);
        modeloPalanca->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        modeloPalanca->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Palanca agregada en el pasillo frontal" << std::endl;
        modeloPalancaPtr = modeloPalanca;
    }
    modelosExtra.push_back(modeloPalanca);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* modeloCajaPalanca = nullptr;
    if (!modeloCajaPalanca) {
        modeloCajaPalanca = new Model("models/crowbar/crowbarbox/crowbarbox.obj");
        modeloCajaPalanca->position = glm::vec3(37.5f, sueloY + 3.0f, 37.0f);
        modeloCajaPalanca->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
        modeloCajaPalanca->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Caja de palanca agregada en el pasillo frontal" << std::endl;
    }
    modelosExtra.push_back(modeloCajaPalanca);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    float lucesXF[2] = {30.0f, 35.0f};
    for(int i = 0; i < 2; i++) {
        glm::vec3 posLuzF = glm::vec3(lucesXF[i], alturaLampara, zCentroF);

        ObjetoFisico panelLuzF;
        panelLuzF.posicion = posLuzF;
        panelLuzF.escala = glm::vec3(1.0f, 0.1f, 2.5f);
        panelLuzF.color = glm::vec3(0.9f, 0.9f, 0.95f);
        panelLuzF.tieneColision = false;
        panelLuzF.esPuerta = false;
        objetos.push_back(panelLuzF);

        LuzPuntual luzF;
        luzF.posicion = posLuzF - glm::vec3(0.0f, 0.1f, 0.0f);
        luzF.intensidad = 1.2f;
        luzF.color = glm::vec3(0.7f, 0.7f, 0.8f);
        luzF.parpadea = false;
        luzF.direction = glm::vec3(0.0f, -1.0f, 0.0f);
        luzF.cutOff = glm::cos(glm::radians(45.0f));
        luzF.outerCutOff = glm::cos(glm::radians(60.0f));
        luzF.tiempoParpadeo = 0.0f;
        luzF.offsetParpadeo = 0.0f;
        luzF.diffuseStrength = 0.4f;
        luces.push_back(luzF);
    }

    float xCentroD = 18.5f;
    float zCentroD = 51.0f;
    float largoPasilloD = 10.0f;
    float zInicioPasilloD = 46.0f;

    ObjetoFisico sueloD;
    sueloD.posicion = glm::vec3(xCentroD, sueloY, zCentroD);
    sueloD.escala = glm::vec3(3.0f, 0.05f, largoPasilloD);
    sueloD.color = colSuelo;
    sueloD.tieneColision = true;
    sueloD.esPuerta = false;
    sueloD.tieneTextura = true;
    sueloD.texturaID = texturaSuelo;
    objetos.push_back(sueloD);

    ObjetoFisico techoD = sueloD;
    techoD.posicion.y = techoY;
    techoD.color = colPared;
    objetos.push_back(techoD);

    ObjetoFisico pDerPasilloD;
    pDerPasilloD.posicion = glm::vec3(17.0f, 0.0f, zCentroD);
    pDerPasilloD.escala = glm::vec3(0.1f, alto, largoPasilloD);
    pDerPasilloD.color = colPared;
    pDerPasilloD.tieneColision = true;
    pDerPasilloD.esPuerta = false;
    pDerPasilloD.tieneTextura = true;
    pDerPasilloD.texturaID = texturaPared2;
    objetos.push_back(pDerPasilloD);

    ObjetoFisico pIzqPasilloD;
    pIzqPasilloD.posicion = glm::vec3(20.0f, 0.0f, zCentroD);
    pIzqPasilloD.escala = glm::vec3(0.1f, alto, largoPasilloD);
    pIzqPasilloD.color = colPared;
    pIzqPasilloD.tieneColision = true;
    pIzqPasilloD.esPuerta = false;
    pIzqPasilloD.tieneTextura = true;
    pIzqPasilloD.texturaID = texturaPared2;
    objetos.push_back(pIzqPasilloD);

    ObjetoFisico pFondoD1;
    pFondoD1.posicion = glm::vec3(17.2f, 0.0f, 56.0f);
    pFondoD1.escala = glm::vec3(0.4f, alto, 0.1f);
    pFondoD1.color = colPared;
    pFondoD1.tieneColision = true;
    pFondoD1.esPuerta = false;
    pFondoD1.tieneTextura = true;
    pFondoD1.texturaID = texturaPared2;
    objetos.push_back(pFondoD1);

    ObjetoFisico pFondoD2;
    pFondoD2.posicion = glm::vec3(19.8f, 0.0f, 56.0f);
    pFondoD2.escala = glm::vec3(0.4f, alto, 0.1f);
    pFondoD2.color = colPared;
    pFondoD2.tieneColision = true;
    pFondoD2.esPuerta = false;
    pFondoD2.tieneTextura = true;
    pFondoD2.texturaID = texturaPared2;
    objetos.push_back(pFondoD2);

    ObjetoFisico pFondoSupD;
    pFondoSupD.posicion = glm::vec3(18.5f, sueloY + pAlto + (alto - pAlto)/2.0f, 56.0f);
    pFondoSupD.escala = glm::vec3(2.2f, alto - pAlto, 0.1f);
    pFondoSupD.color = colPared;
    pFondoSupD.tieneColision = true;
    pFondoSupD.esPuerta = false;
    pFondoSupD.tieneTextura = true;
    pFondoSupD.texturaID = texturaPared2;
    objetos.push_back(pFondoSupD);

    puertasMadera.push_back(Puerta(glm::vec3(18.5f, sueloY + 1.5f, 56.0f), 2.2f, 7.0f, 0.1f, 0.0f));
    puertasMadera.back().setTextura("Textures/PuertaMadera.png");
    puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");

    LucesRectangulares(glm::vec3(xCentroD, alturaLampara, zInicioPasilloD + (largoPasilloD / 3.0f)));
    LucesRectangulares(glm::vec3(xCentroD, alturaLampara, zInicioPasilloD + (largoPasilloD * 2.0f / 3.0f)));
}

void Escenario::Recepcion() {
    float sueloY = -alto / 2.0f;
    float techoY = alto / 2.0f;
    float pAlto = 5.0f;
    glm::vec3 colPared = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 colSuelo = glm::vec3(0.15f, 0.15f, 0.15f);

    float zInicio = 56.0f;
    float largoArea = 18.0f;
    float anchoArea = 18.0f;
    float xCentro = 18.5f;
    float zCentro = zInicio + (largoArea / 2.0f);
    float zFin = zInicio + largoArea;

    ObjetoFisico suelo;
    suelo.posicion = glm::vec3(xCentro, sueloY, zCentro);
    suelo.escala = glm::vec3(anchoArea, 0.05f, largoArea);
    suelo.color = colSuelo;
    suelo.tieneColision = true;
    suelo.esPuerta = false;
    suelo.tieneTextura = true;
    suelo.texturaID = texturaSueloAlfombra;
    objetos.push_back(suelo);

    ObjetoFisico techo = suelo;
    techo.posicion.y = techoY;
    techo.color = colPared;
    techo.tieneColision = true;
    techo.texturaID = texturatecho;
    objetos.push_back(techo);

    ObjetoFisico hitbox;

    ObjetoFisico pAtrasIzq;
    pAtrasIzq.posicion = glm::vec3(13.25f, 0.0f, zInicio);
    pAtrasIzq.escala = glm::vec3(7.5f, alto, 0.1f);
    pAtrasIzq.color = colPared;
    pAtrasIzq.tieneColision = true;
    pAtrasIzq.esPuerta = false;
    pAtrasIzq.tieneTextura = true;
    pAtrasIzq.texturaID = texturaPared3;
    objetos.push_back(pAtrasIzq);

    ObjetoFisico pAtrasDer;
    pAtrasDer.posicion = glm::vec3(23.75f, 0.0f, zInicio);
    pAtrasDer.escala = glm::vec3(7.5f, alto, 0.1f);
    pAtrasDer.color = colPared;
    pAtrasDer.tieneColision = true;
    pAtrasDer.esPuerta = false;
    pAtrasDer.tieneTextura = true;
    pAtrasDer.texturaID = texturaPared3;
    objetos.push_back(pAtrasDer);

    ObjetoFisico pIzq;
    pIzq.posicion = glm::vec3(xCentro - anchoArea/2.0f, 0.0f, zCentro);
    pIzq.escala = glm::vec3(0.1f, alto, largoArea);
    pIzq.color = colPared;
    pIzq.tieneColision = true;
    pIzq.esPuerta = false;
    pIzq.tieneTextura = true;
    pIzq.texturaID = texturaPared3;
    objetos.push_back(pIzq);

    ObjetoFisico pDer;
    pDer.posicion = glm::vec3(xCentro + anchoArea/2.0f, 0.0f, zCentro);
    pDer.escala = glm::vec3(0.1f, alto, largoArea);
    pDer.color = colPared;
    pDer.tieneColision = true;
    pDer.esPuerta = false;
    pDer.tieneTextura = true;
    pDer.texturaID = texturaPared3;
    objetos.push_back(pDer);

    ObjetoFisico pFrenteIzq;
    pFrenteIzq.posicion = glm::vec3(13.45f, 0.0f, zFin);
    pFrenteIzq.escala = glm::vec3(7.9f, alto, 0.1f);
    pFrenteIzq.color = colPared;
    pFrenteIzq.tieneColision = true;
    pFrenteIzq.esPuerta = false;
    pFrenteIzq.tieneTextura = true;
    pFrenteIzq.texturaID = texturaPared3;
    objetos.push_back(pFrenteIzq);

    ObjetoFisico pFrenteDer;
    pFrenteDer.posicion = glm::vec3(23.55f, 0.0f, zFin);
    pFrenteDer.escala = glm::vec3(7.9f, alto, 0.1f);
    pFrenteDer.color = colPared;
    pFrenteDer.tieneColision = true;
    pFrenteDer.esPuerta = false;
    pFrenteDer.tieneTextura = true;
    pFrenteDer.texturaID = texturaPared3;
    objetos.push_back(pFrenteDer);

    ObjetoFisico pFrenteSup;
    pFrenteSup.posicion = glm::vec3(18.5f, sueloY + pAlto + (alto - pAlto)/2.0f, zFin);
    pFrenteSup.escala = glm::vec3(2.2f, alto - pAlto, 0.1f);
    pFrenteSup.color = colPared;
    pFrenteSup.tieneColision = true;
    pFrenteSup.esPuerta = false;
    pFrenteSup.tieneTextura = true;
    pFrenteSup.texturaID = texturaPared3;
    objetos.push_back(pFrenteSup);

    unsigned int texturaMan = cargarTextura("textures/Cuadro1.png");
    unsigned int texturaMan2 = cargarTextura("textures/Cuadro2.png");

    ObjetoFisico objetoMan;
    objetoMan.posicion = glm::vec3(xCentro - 5.0f, sueloY + 4.5f, zCentro + 8.8f);
    objetoMan.escala = glm::vec3(4.0f, 5.0f, 0.05f);
    objetoMan.rotacionY = 45.0f;
    objetoMan.tieneTextura = true;
    objetoMan.texturaID = texturaMan;
    objetoMan.tieneColision = true;
    objetoMan.esPuerta = false;
    objetos.push_back(objetoMan);

    std::cout << "Objeto con textura man.png agregado en AreaFinal1" << std::endl;

    objetoMan.posicion = glm::vec3(xCentro + 5.0f, sueloY + 4.5f, zCentro + 8.8f);
    objetoMan.escala = glm::vec3(4.0f, 5.0f, 0.05f);
    objetoMan.rotacionY = 45.0f;
    objetoMan.tieneTextura = true;
    objetoMan.texturaID = texturaMan2;
    objetoMan.tieneColision = true;
    objetoMan.esPuerta = false;
    objetos.push_back(objetoMan);

    puertasMadera.push_back(Puerta(glm::vec3(18.5f, sueloY + 1.5f, zFin), 2.2f, 7.0f, 0.1f, 0.0f));
    puertasMadera.back().setTextura("Textures/PuertaMadera.png");
    puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");

    static Model* modeloMesaBase = nullptr;
    static Model* modeloSillaBase = nullptr;
    static Model* modeloPantallaBase = nullptr;
    static Model* modeloChairBase = nullptr;

    if (!modeloMesaBase) {
        modeloMesaBase = new Model("models/MesaTrabajo/MesaTrabajo.obj");
        modeloMesaBase->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Modelo de Mesa base cargado para NuevaAreaF" << std::endl;
    }

    if (!modeloSillaBase) {
        modeloSillaBase = new Model("models/Silla/Silla.obj");
        modeloSillaBase->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Modelo de Silla base cargado para NuevaAreaF" << std::endl;
    }

    if (!modeloPantallaBase) {
        modeloPantallaBase = new Model("models/Pantalla/Pantalla.obj");
        modeloPantallaBase->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Modelo de Pantalla base cargado para NuevaAreaF" << std::endl;
    }

    if (!modeloChairBase) {
        modeloChairBase = new Model("models/chair/Chair.obj");
        modeloChairBase->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Modelo de Chair base cargado para NuevaAreaF" << std::endl;
    }

    Model* mesa = new Model(*modeloMesaBase);
    mesa->position = glm::vec3(xCentro + 5.0f, sueloY, zCentro - 5.0f);
    mesa->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    mesa->scale = glm::vec3(1.0f, 1.2f, 1.0f);
    modelosExtra.push_back(mesa);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    modelosExtra.push_back(mesa);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    hitbox.posicion = glm::vec3(xCentro + 5.0f, sueloY, zCentro - 5.9f);
    hitbox.escala   = glm::vec3(5.9f, 10.0f, 3.8f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    Model* silla = new Model(*modeloSillaBase);
    silla->position = glm::vec3(xCentro + 2.0f, sueloY, zCentro - 6.5f);
    silla->rotation = glm::vec3(0.0f, 180.0f, 0.0f);
    silla->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    modelosExtra.push_back(silla);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    Model* pantalla = new Model(*modeloPantallaBase);
    pantalla->position = glm::vec3(xCentro + 5.0f, sueloY + 0.5f, zFin - 14.0f);
    pantalla->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    pantalla->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    modelosExtra.push_back(pantalla);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    Model* chair1 = new Model(*modeloChairBase);
    chair1->position = glm::vec3(xCentro + 7.5f, sueloY, zCentro - 1.5f);
    chair1->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    chair1->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    modelosExtra.push_back(chair1);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    Model* chair2 = new Model(*modeloChairBase);
    chair2->position = glm::vec3(xCentro + 7.5f, sueloY, zCentro + 1.5f);
    chair2->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    chair2->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    modelosExtra.push_back(chair2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    Model* chair3 = new Model(*modeloChairBase);
    chair3->position = glm::vec3(xCentro + 7.5f, sueloY, zCentro + 4.5f);
    chair3->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    chair3->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    modelosExtra.push_back(chair3);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    hitbox.posicion = glm::vec3(xCentro + 7.5f, sueloY, zCentro + 1.5f);
    hitbox.escala   = glm::vec3(2.9f, 10.0f, 7.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    Model* chair4 = new Model(*modeloChairBase);
    chair4->position = glm::vec3(xCentro - 7.5f, sueloY, zCentro - 1.5f);
    chair4->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
    chair4->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    modelosExtra.push_back(chair4);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    Model* chair5 = new Model(*modeloChairBase);
    chair5->position = glm::vec3(xCentro - 7.5f, sueloY, zCentro + 1.5f);
    chair5->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
    chair5->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    modelosExtra.push_back(chair5);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    Model* chair6 = new Model(*modeloChairBase);
    chair6->position = glm::vec3(xCentro - 7.5f, sueloY, zCentro + 4.5f);
    chair6->rotation = glm::vec3(0.0f, 90.0f, 0.0f);
    chair6->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    modelosExtra.push_back(chair6);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    hitbox.posicion = glm::vec3(xCentro - 7.5f, sueloY, zCentro + 1.5f);
    hitbox.escala   = glm::vec3(2.9f, 10.0f, 7.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* Maceta2 = nullptr;
    if (!Maceta2) {
        Maceta2 = new Model("models/Maceta/Maceta.obj");
        Maceta2->position = glm::vec3(xCentro - 7.0f, sueloY, zCentro - 7.0f);
        Maceta2->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        Maceta2->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 3 agregada" << std::endl;
    }
    modelosExtra.push_back(Maceta2);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    hitbox.posicion = glm::vec3(xCentro - 7.0f, sueloY, zCentro -7.0f);
    hitbox.escala   = glm::vec3(1.5f, 10.0f, 1.5f);
    hitbox.tieneColision = true;
    hitbox.tieneTextura = false;
    hitbox.visible = false;
    objetos.push_back(hitbox);

    static Model* Papel6 = nullptr;
    if (!Papel6) {
        Papel6 = new Model("models/Papeles/Papel2.obj");
        Papel6->position = glm::vec3(xCentro + 6.5f, sueloY - 1.7f, zCentro - 6.5f);
        Papel6->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        Papel6->scale = glm::vec3(2.0f, 2.0f, 2.0f);
        std::cout << "Mesa 3 agregada" << std::endl;
    }
    modelosExtra.push_back(Papel6);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    static Model* Teclado3 = nullptr;
    if (!Teclado3) {
        Teclado3 = new Model("models/keyboard/Teclado.obj");
        Teclado3->position = glm::vec3(xCentro + 4.4f, sueloY + 0.5, zCentro - 5.5f);
        Teclado3->rotation = glm::vec3(0.0f, 180.0f, 0.0f);
        Teclado3->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        std::cout << "Mesa 3 agregada" << std::endl;
    }
    modelosExtra.push_back(Teclado3);
    modelosPosiciones.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelosEscalas.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    float alturaLampara = techoY - 0.05f;
    float lucesX[2] = {14.0f, 23.0f};
    float lucesZ[2] = {60.5f, 69.5f};

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            LucesRectangulares(glm::vec3(lucesX[i], alturaLampara, lucesZ[j]));
        }
    }
}

void Escenario::PasilloRecepcion() {
    float sueloY = -alto / 2.0f;
    float altoPasillo = 7.0f;
    float techoPasillo = sueloY + altoPasillo;
    float anchoPasillo = 10.0f;
    float largoPasillo = 40.0f;
    float zInicio = 74.0f;
    float xCentro = 18.5f;
    float zFin = zInicio + largoPasillo;

    glm::vec3 colPared = glm::vec3(0.22f, 0.22f, 0.24f);
    glm::vec3 colSuelo = glm::vec3(0.12f, 0.12f, 0.14f);

    auto crearBloque = [&](glm::vec3 pos, glm::vec3 esc, glm::vec3 col, bool colision, bool textura, int texID) {
        ObjetoFisico obj;
        obj.posicion = pos; obj.escala = esc; obj.color = col;
        obj.tieneColision = colision; obj.tieneTextura = textura; obj.texturaID = texID;
        objetos.push_back(obj);
    };

    // 1. SUELO Y TECHO
    crearBloque(glm::vec3(xCentro, sueloY, zInicio + largoPasillo/2.0f), glm::vec3(anchoPasillo, 0.05f, largoPasillo), colSuelo, true, true, texturaSuelo);
    crearBloque(glm::vec3(xCentro, techoPasillo, zInicio + largoPasillo/2.0f), glm::vec3(anchoPasillo, 0.05f, largoPasillo), colPared, true, true, texturatecho);

    // 2. PAREDES LATERALES
    float pAncho = 2.2f;
    float pAlto = 5.0f;
    float separacionPuertas = 10.0f;
    float primeraPuertaZ = zInicio + 5.0f;
    float xLados[2] = {xCentro - anchoPasillo/2.0f, xCentro + anchoPasillo/2.0f};

    for (float x : xLados) {
        float zActual = zInicio;
        for (int i = 0; i < 3; i++) {
            float zPuerta = primeraPuertaZ + (i * separacionPuertas);

            float largoSeg = (zPuerta - pAncho/2.0f) - zActual;
            if (largoSeg > 0.01f) crearBloque(glm::vec3(x, sueloY + altoPasillo/2.0f, zActual + largoSeg/2.0f), glm::vec3(0.1f, altoPasillo, largoSeg), colPared, true, true, texturaPared2);

            crearBloque(glm::vec3(x, sueloY + pAlto + (altoPasillo - pAlto)/2.0f, zPuerta), glm::vec3(0.1f, altoPasillo - pAlto, pAncho), colPared, true, true, texturaPared2);

            float rot = (x < xCentro) ? 90.0f : 270.0f;
            Puerta nuevaPuerta(glm::vec3(x, sueloY + pAlto/2.0f, zPuerta), pAncho, pAlto, 0.1f, rot);
            nuevaPuerta.setBloqueada(true);
            puertasMadera.push_back(nuevaPuerta);
            puertasMadera.back().setTextura("Textures/PuertaMadera.png");
            puertasMadera.back().setTexturaPicaporte("Textures/Picaporte.png");

            zActual = zPuerta + pAncho/2.0f;
        }
        if (zActual < zFin) crearBloque(glm::vec3(x, sueloY + altoPasillo/2.0f, (zActual + zFin)/2.0f), glm::vec3(0.1f, altoPasillo, zFin - zActual), colPared, true, true, texturaPared2);
    }

    // 3. FONDO Y PUERTA CRISTAL
    float pCristalAncho = 3.0f; float pCristalAlto = 6.0f;
    float margen = (anchoPasillo - pCristalAncho) / 2.0f;

    crearBloque(glm::vec3(xCentro - pCristalAncho/2.0f - margen/2.0f, sueloY + altoPasillo/2.0f, zFin), glm::vec3(margen, altoPasillo, 0.1f), colPared, true, true, texturaPared2);
    crearBloque(glm::vec3(xCentro + pCristalAncho/2.0f + margen/2.0f, sueloY + altoPasillo/2.0f, zFin), glm::vec3(margen, altoPasillo, 0.1f), colPared, true, true, texturaPared2);
    crearBloque(glm::vec3(xCentro, sueloY + pCristalAlto + (altoPasillo - pCristalAlto)/2.0f, zFin), glm::vec3(pCristalAncho, altoPasillo - pCristalAlto, 0.1f), colPared, true, true, texturaPared2);

    // Cristales
    crearBloque(glm::vec3(xCentro - pCristalAncho/4.0f, sueloY + pCristalAlto/2.0f, zFin + 0.02f), glm::vec3(pCristalAncho/2.0f - 0.05f, pCristalAlto - 0.2f, 0.04f), glm::vec3(0.6f, 0.8f, 0.9f), true, false, 0);
    crearBloque(glm::vec3(xCentro + pCristalAncho/4.0f, sueloY + pCristalAlto/2.0f, zFin + 0.02f), glm::vec3(pCristalAncho/2.0f - 0.05f, pCristalAlto - 0.2f, 0.04f), glm::vec3(0.6f, 0.8f, 0.9f), true, false, 0);

    // 4. LUCES
    float alturaLampara = techoPasillo - 0.05f;
    float lucesZ[4] = {zInicio + 8.0f, zInicio + 16.0f, zInicio + 24.0f, zInicio + 32.0f};
    for(int i = 0; i < 4; i++) {
        LucesRectangulares(glm::vec3(xCentro, alturaLampara, lucesZ[i]));
    }

    // 5. CARTEL SALIDA CON LUZ PARPADEANTE
    crearBloque(glm::vec3(xCentro, sueloY + pCristalAlto + 0.5f, zFin - 0.06f), glm::vec3(1.0f, 0.4f, 0.05f), glm::vec3(0.0f, 0.9f, 0.2f), false, false, 0);

    LuzPuntual luzExit;
    luzExit.posicion = glm::vec3(xCentro, sueloY + pCristalAlto + 0.5f, zFin - 0.12f);
    luzExit.intensidad = 0.6f;
    luzExit.color = glm::vec3(0.1f, 1.0f, 0.2f);
    luzExit.parpadea = true;
    luzExit.direction = glm::vec3(0.0f, -0.2f, -1.0f);
    luzExit.cutOff = glm::cos(glm::radians(60.0f));
    luzExit.outerCutOff = glm::cos(glm::radians(75.0f));
    luzExit.tiempoParpadeo = 0.0f;
    luzExit.offsetParpadeo = (float)(rand() % 100) / 50.0f;
    luzExit.visible = false;
    luzExit.diffuseStrength = 1.0f;
    luces.push_back(luzExit);
}

void Escenario::crearIndicadoresProgreso() {
    glm::vec3 posPalanca(37.5f, -2.0f, 37.0f);
    float separacion = 0.6f;
    float anchoVisible = 0.3f;
    float alto = 0.3f;
    float grosor = 0.05f;

    float xPared = 37.4f;

    for (int i = 0; i < 3; i++) {
        ObjetoFisico cuadro;
        cuadro.posicion = glm::vec3(xPared, posPalanca.y + 1.5f, posPalanca.z + (i - 1) * separacion);
        cuadro.escala = glm::vec3(grosor, alto, anchoVisible);
        cuadro.color = glm::vec3(0.2f);
        cuadro.tieneColision = false;
        cuadro.tieneTextura = false;
        cuadro.visible = true;
        cuadro.rotacionY = 90.0f;
        cuadro.esIndicador = true;
        objetos.push_back(cuadro);
        indicesIndicadoresProgreso.push_back(objetos.size() - 1);
    }
}

void Escenario::actualizarIndicadoresProgreso() {
    glm::vec3 colorEncendido(1.0f, 1.0f, 1.0f);
    glm::vec3 colorApagado(0.2f, 0.2f, 0.2f);

    for (size_t i = 0; i < indicesIndicadoresProgreso.size(); i++) {
        int idx = indicesIndicadoresProgreso[i];
        if (idx >= 0 && idx < (int)objetos.size()) {
            if ((int)i < secuenciaActual) {
                objetos[idx].color = colorEncendido;
            } else {
                objetos[idx].color = colorApagado;
            }
        }
    }
}

// ==================== MÉTODOS DE NOTAS ====================

void Escenario::agregarNota(const glm::vec3& pos, const glm::vec3& esc, const std::string& texto, float radioInteraccion) {
    Nota nota(pos, esc, texto);
    nota.setTextura(texturaNota);
    nota.setRadioInteraccion(radioInteraccion);
    notas.push_back(nota);
}

bool Escenario::jugadorCercaNota(const glm::vec3& posJugador, std::string& textoSalida, int& indice) const {
    for (size_t i = 0; i < notas.size(); ++i) {
        const Nota& n = notas[i];
        if (!n.isVisible()) continue;
        float dx = posJugador.x - n.getPosicion().x;
        float dz = posJugador.z - n.getPosicion().z;
        float dist = sqrt(dx*dx + dz*dz);
        if (dist < n.getRadioInteraccion()) {
            textoSalida = n.getTexto();
            indice = (int)i;
            return true;
        }
    }
    return false;
}

void Escenario::marcarNotaLeida(int indice) {
    if (indice >= 0 && indice < (int)notas.size()) {
        notas[indice].setLeida(true);
    }
}

void Escenario::renderNotas(Shader& shader) {
    if (notas.empty()) return;
    shader.use();
    shader.setBool("usarTextura", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaNota);
    shader.setInt("textura", 0);

    glBindVertexArray(cuboVAO);
    for (const auto& nota : notas) {
        if (!nota.isVisible()) continue;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, nota.getPosicion());
        model = glm::scale(model, nota.getEscala());
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

// ==================== MÉTODOS DE BOTONES ====================

void Escenario::agregarBoton(const glm::vec3& pos, const glm::vec3& esc, const glm::vec3& col, int orden, float radio) {
    Boton boton(pos, esc, col, orden);
    boton.setRadioInteraccion(radio);
    botones.push_back(boton);
}

bool Escenario::jugadorCercaBoton(const glm::vec3& posJugador, int& indice) {
    for (size_t i = 0; i < botones.size(); ++i) {
        Boton& b = botones[i];
        if (b.isActivo()) continue;
        float dx = posJugador.x - b.getPosicion().x;
        float dz = posJugador.z - b.getPosicion().z;
        float dist = sqrt(dx*dx + dz*dz);
        if (dist < b.getRadioInteraccion()) {
            indice = i;
            return true;
        }
    }
    return false;
}

void Escenario::presionarBoton(int indice) {
    if (indice < 0 || indice >= (int)botones.size()) return;
    Boton& b = botones[indice];

    if (b.getOrdenRequerido() == 4) {
        if (botonSalidaPresionado) {
            std::cout << "El botón de salida ya ha sido presionado." << std::endl;
            return;
        }
        if (!puzzleResuelto) {
            std::cout << "El botón de salida no tiene energía. Resuelve el puzzle principal primero." << std::endl;
            return;
        }
        abrirPuertaSalida();
        botonSalidaPresionado = true;
        b.setActivo(true);
        this->activarMonstruo(glm::vec3(18.5f, -4.5f, 37.0f));
        std::cout << "[SISTEMA] ¡El monstruo ha despertado en el Area de Seguridad!" << std::endl;

        std::cout << "¡Has presionado el botón de salida! La puerta de salida se ha abierto." << std::endl;
        return;
    }

    if (b.isActivo()) return;

    if (b.getOrdenRequerido() == secuenciaActual + 1) {
        b.setActivo(true);
        secuenciaActual++;
        actualizarIndicadoresProgreso();
        std::cout << "Botón " << b.getOrdenRequerido() << " activado (" << secuenciaActual << "/3)" << std::endl;

        if (secuenciaActual == 3) {
            puzzleResuelto = true;
            palancaActivable = true;
            actualizarIndicadoresProgreso();
            std::cout << "¡Puzzle completado! La palanca ahora tiene energía." << std::endl;
        }
    } else {
        std::cout << "Orden incorrecto. Reiniciando puzzle." << std::endl;
        for (auto& btn : botones) {
            btn.reiniciar();
        }
        secuenciaActual = 0;
        puzzleResuelto = false;
        palancaActivable = false;
        actualizarIndicadoresProgreso();
    }
}

void Escenario::renderBotones(Shader& shader) {
    if (botones.empty()) return;
    shader.use();
    shader.setBool("usarTextura", false);
    glBindVertexArray(cuboVAO);
    for (const auto& btn : botones) {
        if (!btn.isVisible()) continue;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, btn.getPosicion());
        model = glm::scale(model, btn.getEscala());
        shader.setMat4("model", model);
        shader.setVec3("objectColor", btn.getColor());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void Escenario::abrirPuertaJefe() {
    puertaJefeBloqueada = false;
    std::cout << "La puerta de la oficina del jefe se ha desbloqueado." << std::endl;
}

void Escenario::abrirPuertaSalida() {
    puertaSalidaBloqueada = false;
    std::cout << "¡La puerta de salida se ha abierto!" << std::endl;
}
