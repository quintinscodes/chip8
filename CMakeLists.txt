cmake_minimum_required(VERSION 3.10)

# Nombre de tu proyecto
project(Chip8Emulator)

# Usar el estándar C++17 o superior
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar SDL2 en el sistema
find_package(SDL2 REQUIRED)

# --- glad ---
# Carpeta donde pusiste el zip generado en gen.glad.sh
add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

# Definir el ejecutable (nombre del programa y archivos fuente)
add_executable(chip8_emu chip8.cpp main.cpp platform.cpp)

# Enlazar SDL2 y glad a tu ejecutable
target_include_directories(chip8_emu PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(chip8_emu PRIVATE ${SDL2_LIBRARIES} glad)

# En Linux, glad necesita poder cargar símbolos de OpenGL/dl
if(UNIX AND NOT APPLE)
    target_link_libraries(chip8_emu PRIVATE dl)
endif()