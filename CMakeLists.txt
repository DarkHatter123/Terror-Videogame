cmake_minimum_required(VERSION 3.20)
project(Proyecto_Terror)

set(CMAKE_CXX_STANDARD 20)

# ========================
# DEFINICIONES
# ========================
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)

# ========================
# EJECUTABLE
# ========================
add_executable(Proyecto_Terror
        main.cpp
        Shader.cpp
        Escenario.cpp
        Mesh.cpp
        Model.cpp
        Lampara.cpp
        Puerta.cpp
        Caja.cpp
        TexturaUtils.cpp
        Menu.cpp
        Menu2D.cpp

        Mesh.h
        Model.h
        Lampara.h
        Puerta.h
        Caja.h

        C:/Librerias/glad/src/glad.c

        C:/Librerias/SOIL2/src/SOIL2/SOIL2.c
        C:/Librerias/SOIL2/src/SOIL2/image_DXT.c
        C:/Librerias/SOIL2/src/SOIL2/image_helper.c
        C:/Librerias/SOIL2/src/SOIL2/wfETC.c
        C:/Librerias/SOIL2/src/SOIL2/image_array.c


        # IMGUI
        C:/Librerias/imgui-master/imgui.cpp
        C:/Librerias/imgui-master/imgui_draw.cpp
        C:/Librerias/imgui-master/imgui_tables.cpp
        C:/Librerias/imgui-master/imgui_widgets.cpp
        C:/Librerias/imgui-master/imgui_demo.cpp
        C:/Librerias/imgui-master/backends/imgui_impl_glfw.cpp
        C:/Librerias/imgui-master/backends/imgui_impl_opengl3.cpp
)

# ========================
# INCLUDE DIRECTORIES
# ========================
include_directories("C:/Librerias/glm")
include_directories("C:/Librerias/assimp/build/include")  # PARA CONFIG.H
include_directories("C:/Librerias/assimp/include")        # PARA IMPORTER.HPP
include_directories("C:/Librerias/imgui-master")
include_directories("C:/Librerias/imgui-master/backends")

target_include_directories(Proyecto_Terror PRIVATE
        C:/Librerias/glfw/include
        C:/Librerias/glad/include
        C:/Librerias/glm
        C:/Librerias/SFML/include
        C:/Librerias/SOIL2/
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

        C:/Librerias/SFML/lib/libsfml-audio.a
        C:/Librerias/SFML/lib/libsfml-system.a

        # Opcional (si luego usas ventanas o gráficos de SFML)
        # C:/Librerias/SFML/lib/libsfml-window.a
        # C:/Librerias/SFML/lib/libsfml-graphics.a
)

# ========================
# DEFINICIONES PARA IMGUI
# ========================
target_compile_definitions(Proyecto_Terror PRIVATE
        IMGUI_IMPL_OPENGL_LOADER_GLAD
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