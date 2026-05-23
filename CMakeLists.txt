cmake_minimum_required(VERSION 3.20)
project(Proyecto_Terror)

set(CMAKE_CXX_STANDARD 20)

# ========================
# EJECUTABLE
# ========================
add_executable(Proyecto_Terror
        main.cpp
        Shader.cpp
        Skybox.cpp
        Escenario.cpp
        Mesh.cpp
        Model.cpp
        stb_image.cpp
        Lampara.cpp

        Mesh.h
        Model.h
        Lampara.h

        C:/Librerias/glad/src/glad.c
)

# ========================
# INCLUDE DIRECTORIES
# ========================
include_directories("C:/Librerias/glm")
include_directories("C:/Librerias/assimp/build/include")
include_directories("C:/Librerias/assimp/include")

target_include_directories(Proyecto_Terror PRIVATE
        C:/Librerias/glfw/include
        C:/Librerias/glad/include
        C:/Librerias/stb/include
        C:/Librerias/glm
        C:/Librerias/SFML/include
)

# ========================
# LIB DIRECTORIES
# ========================
link_directories(C:/Librerias/assimp/build/lib)
link_directories(C:/Librerias/SFML/lib)

# ========================
# LINK LIBRARIES
# ========================
target_link_libraries(Proyecto_Terror PRIVATE

        # GLFW
        C:/Librerias/glfw/lib-mingw-w64/libglfw3.a

        # OpenGL
        opengl32
        gdi32
        user32

        # Assimp
        C:/Librerias/assimp/build/lib/libassimp.dll.a

        # SFML AUDIO
        C:/Librerias/SFML/lib/libsfml-audio.a
        C:/Librerias/SFML/lib/libsfml-system.a

        # Opcional (si luego usas ventanas o gráficos de SFML)
        # C:/Librerias/SFML/lib/libsfml-window.a
        # C:/Librerias/SFML/lib/libsfml-graphics.a
)

# ========================
# COPIAR SHADERS
# ========================
file(COPY
        ${CMAKE_SOURCE_DIR}/shaders/skybox.vert
        ${CMAKE_SOURCE_DIR}/shaders/skybox.frag
        ${CMAKE_SOURCE_DIR}/shaders/escenario.vert
        ${CMAKE_SOURCE_DIR}/shaders/escenario.frag
        DESTINATION ${CMAKE_BINARY_DIR}/shaders
)

# ========================
# COPIAR TEXTURAS / MODELOS
# ========================
# Ejemplo:
#
# file(COPY
#         ${CMAKE_SOURCE_DIR}/textures
#         DESTINATION ${CMAKE_BINARY_DIR}
# )
#
# file(COPY
#         ${CMAKE_SOURCE_DIR}/models
#         DESTINATION ${CMAKE_BINARY_DIR}
# )