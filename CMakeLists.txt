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
set(ASSIMP_BUILD "C:/assimp-master/build")
set(SFML_PATH "C:/SFML")

# Dear ImGui
set(IMGUI_ROOT "C:/imgui-master/imgui-master")

# ========================
# DEFINICIONES
# ========================
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)

# ========================
# INCLUDES
# ========================
include_directories(
        ${GLFW_PATH}/include
        ${GLM_PATH}
        ${GLAD_PATH}/include
        ${ASSIMP_ROOT}/include
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/SOIL2
        ${SFML_PATH}

        ${IMGUI_ROOT}
        ${IMGUI_ROOT}/backends
)

# ========================
# LIBRERÍAS
# ========================
link_directories(
        ${GLFW_PATH}/lib-mingw-w64
        ${ASSIMP_BUILD}/lib
        ${SFML_PATH}/lib
)

# ========================
# FUENTES IMGUI
# ========================
set(IMGUI_SOURCES
        ${IMGUI_ROOT}/imgui.cpp
        ${IMGUI_ROOT}/imgui_draw.cpp
        ${IMGUI_ROOT}/imgui_tables.cpp
        ${IMGUI_ROOT}/imgui_widgets.cpp
        ${IMGUI_ROOT}/imgui_demo.cpp

        ${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp
        ${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp
        Menu.cpp
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
        Boton.cpp
        Monstruo.cpp
        MonstruoManager.cpp
        Nota.cpp
        Lampara.cpp
        Lampara.h
        Puerta.cpp
        Puerta.h
        Caja.cpp
        Caja.h
        Menu2D.cpp
        TexturaUtils.cpp

        src/SOIL2/SOIL2.c
        src/SOIL2/image_DXT.c
        src/SOIL2/image_helper.c
        src/SOIL2/wfETC.c
        src/SOIL2/image_array.c

        ${IMGUI_SOURCES}
)

# ========================
# DEFINICIONES IMGUI
# ========================
target_compile_definitions(Proyecto_Terror PRIVATE
        IMGUI_IMPL_OPENGL_LOADER_GLAD
)

# ========================
# LINKEO
# ========================
target_link_libraries(Proyecto_Terror
        C:/Libraries/lib/Libs/libglfw3.a
        opengl32
        gdi32
        user32
        "C:/libraries/include/assimp-master/build/lib/libassimp.dll.a"
        "C:/Libraries/lib/Libs/libsfml-graphics.a"
        "C:/Libraries/lib/Libs/libsfml-window.a"
        "C:/Libraries/lib/Libs/libsfml-system.a"
        "C:/Libraries/lib/Libs/libsfml-audio.a"
)