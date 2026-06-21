#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "src/SOIL2/SOIL2.h"
#include "Shader.h"

class Skybox
{
private:
    unsigned int skyboxVAO, skyboxVBO, cubemapTexture;
    Shader skyboxShader; // El shader se maneja internamente

    void setupSkybox()
    {
        // Se mantiene el tamaño original de 10.0f
        float skyboxVertices[] = {
            -10.0f,  10.0f, -10.0f,
            -10.0f, -10.0f, -10.0f,
             10.0f, -10.0f, -10.0f,
             10.0f, -10.0f, -10.0f,
             10.0f,  10.0f, -10.0f,
            -10.0f,  10.0f, -10.0f,

            -10.0f, -10.0f,  10.0f,
            -10.0f, -10.0f, -10.0f,
            -10.0f,  10.0f, -10.0f,
            -10.0f,  10.0f, -10.0f,
            -10.0f,  10.0f,  10.0f,
            -10.0f, -10.0f,  10.0f,

             10.0f, -10.0f, -10.0f,
             10.0f, -10.0f,  10.0f,
             10.0f,  10.0f,  10.0f,
             10.0f,  10.0f,  10.0f,
             10.0f,  10.0f, -10.0f,
             10.0f, -10.0f, -10.0f,

            -10.0f, -10.0f,  10.0f,
            -10.0f,  10.0f,  10.0f,
             10.0f,  10.0f,  10.0f,
             10.0f,  10.0f,  10.0f,
             10.0f, -10.0f,  10.0f,
            -10.0f, -10.0f,  10.0f,

            -10.0f,  10.0f, -10.0f,
             10.0f,  10.0f, -10.0f,
             10.0f,  10.0f,  10.0f,
             10.0f,  10.0f,  10.0f,
            -10.0f,  10.0f,  10.0f,
            -10.0f,  10.0f, -10.0f,

            -10.0f, -10.0f, -10.0f,
            -10.0f, -10.0f,  10.0f,
             10.0f, -10.0f, -10.0f,
             10.0f, -10.0f, -10.0f,
            -10.0f, -10.0f,  10.0f,
             10.0f, -10.0f,  10.0f
        };

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    unsigned int loadCubemap(const std::vector<std::string>& faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, channels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* image = SOIL_load_image(faces[i].c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
            if (image)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                free(image);
            }
            else
            {
                std::cout << "Error cargando textura de Skybox: " << faces[i] << std::endl;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

public:
    // Constructor modificado para aceptar el vector de caras e inicializar tus archivos Shader (.vert y .frag)
    Skybox(const std::vector<std::string>& faces) : skyboxShader("Shaders/skybox.vert", "Shaders/skybox.frag")
    {
        setupSkybox();
        cubemapTexture = loadCubemap(faces);
    }

    ~Skybox()
    {
        glDeleteVertexArrays(1, &skyboxVAO);
        glDeleteBuffers(1, &skyboxVBO);
        glDeleteTextures(1, &cubemapTexture);
    }

    void render(glm::mat4 view, const glm::mat4& projection)
    {
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use(); // <--- cambiado a minúscula 'use'

        view = glm::mat4(glm::mat3(view));

        // Se cambia 'Program' por 'ID' --->
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0); // <--- cambiado a 'ID'
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
    }
};