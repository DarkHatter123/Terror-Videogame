cmake_minimum_required(VERSION 3.29)
project(Proyecto_Terror)

set(CMAKE_CXX_STANDARD 20)

# ========================
# RUTAS
# ========================
set(GLFW_PATH "C:/glfw")
set(GLM_PATH "C:/glm")
set(GLAD_PATH "C:/glad")
set(ASSIMP_ROOT "C:/assimp")
set(ASSIMP_BUILD "C:/assimp/cmake-build-debug")
set(SFML_PATH "C:/SFML")

# ========================
# INCLUDES
# ========================
include_directories(
        ${GLFW_PATH}/include
        ${GLM_PATH}
        ${GLAD_PATH}/include
        ${ASSIMP_ROOT}/include
        ${ASSIMP_BUILD}/include # <-- config.h
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/SOIL2
        ${SFML_PATH}/include

)

# ========================
# LIBRERÍAS
# ========================
link_directories(
        ${GLFW_PATH}/lib-mingw-w64
        ${ASSIMP_BUILD}/lib
        ${SFML_PATH}/lib  # <-- NUEVO (ajústalo si tu carpeta es diferente)
)

# ========================
# EJECUTABLE
# ========================
add_executable(Proyecto_Terror
        main.cpp
        Shader.cpp
        Escenario.cpp
        ${GLAD_PATH}/src/glad.c
        Mesh.cpp
        Mesh.h
        Model.cpp
        Model.h
        Lampara.cpp
        Lampara.h
        Puerta.cpp
        Puerta.h

        src/SOIL2/SOIL2.c
        src/SOIL2/image_DXT.c
        src/SOIL2/image_helper.c
        src/SOIL2/wfETC.c
        src/SOIL2/image_array.c
        Caja.cpp
        Caja.h
)

# ========================
# LINKEO
# ========================
target_link_libraries(Proyecto_Terror
        opengl32
        glfw3
        ${ASSIMP_BUILD}/lib/libassimpd.dll.a
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio  # <-- NUEVO
)

