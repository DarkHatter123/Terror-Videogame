#include "TexturaUtils.h"
#include <src/SOIL2/SOIL2.h>
#include <glad/glad.h>
#include <iostream>

unsigned int cargarTextura(const char* ruta, bool flipVertical) {
    unsigned int textureID;

    // SOIL2 maneja el flip vertical con SOIL_FLAG_INVERT_Y
    unsigned int flags = SOIL_FLAG_MIPMAPS;
    if (flipVertical) {
        flags |= SOIL_FLAG_INVERT_Y;
    }

    textureID = SOIL_load_OGL_texture(
        ruta,
        SOIL_LOAD_RGBA,           // Cargar como RGBA (canal alpha si existe)
        SOIL_CREATE_NEW_ID,
        flags
    );

    if (textureID == 0) {
        std::cout << "ERROR: No se pudo cargar la textura con SOIL2: " << ruta << std::endl;
        std::cout << "Razón: " << SOIL_last_result() << std::endl;

        // Crear textura por defecto blanca si falla
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        unsigned char whitePixel[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Se creó una textura blanca por defecto" << std::endl;
    } else {
        std::cout << "Textura cargada correctamente con SOIL2: " << ruta << std::endl;

        // Configurar parámetros adicionales
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    return textureID;
}