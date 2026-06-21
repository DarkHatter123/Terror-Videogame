#ifndef MENU_H
#define MENU_H

#include <glad/glad.h>
#include <iostream>

class Menu {
private:
    unsigned int textureID;
    bool visible;
    bool enabled;

public:
    Menu();
    ~Menu();

    bool loadTexture(const char* texturePath);
    void render();
    void toggle();
    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; }
    unsigned int getTextureID() const { return textureID; }
    void enable() { enabled = true; }
    void disable() { enabled = false; }
    bool isEnabled() const { return enabled; }
};

#endif