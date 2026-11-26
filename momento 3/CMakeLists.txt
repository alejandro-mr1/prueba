cmake_minimum_required(VERSION 3.16)
project(OperacionBronx VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets Multimedia)

add_executable(OperacionBronx WIN32
    src/main.cpp
    src/geometria.h
    src/personaje.h
    src/personaje.cpp
    src/batman.h
    src/batman.cpp
    src/civil.h
    src/civil.cpp
    src/personajes.h
    src/personajes.cpp
    src/niveles.h
    src/niveles.cpp
    src/partida.h
    src/partida.cpp
    src/vista.h
    src/vista.cpp
    src/ventana.h
    src/ventana.cpp
    src/audio.h
    src/audio.cpp
    recursos/recursos.qrc
)
target_link_libraries(OperacionBronx PRIVATE Qt6::Widgets Qt6::Multimedia)
if(MSVC)
    target_compile_options(OperacionBronx PRIVATE /utf-8 /W4)
else()
    target_compile_options(OperacionBronx PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS OperacionBronx RUNTIME DESTINATION .)
