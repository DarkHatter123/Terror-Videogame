#include "Menu.h"
#include <src/SOIL2/SOIL2.h>
#include <cstdlib>

Menu::Menu()
    : textureID(0), visible(false), enabled(false) {
}

bool Menu::loadTexture(const char* texturePath) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texWidth, texHeight, channels;
    unsigned char* data = SOIL_load_image(texturePath, &texWidth, &texHeight, &channels, SOIL_LOAD_RGBA);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        free(data);
        std::cout << "Textura del mapa cargada: " << texturePath << std::endl;
        return true;
    } else {
        std::cout << "Error: No se pudo cargar " << texturePath << std::endl;
        return false;
    }
}

void Menu::render() {
    // No se usa con ImGui
}

void Menu::toggle() {
    if (enabled) {
        visible = !visible;
    }
}

Menu::~Menu() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}