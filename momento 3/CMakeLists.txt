cmake_minimum_required(VERSION 3.16)
project(OperacionBronx VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets Multimedia)

set(CABECERAS
    accion.h
    aprendizaje.h
    audio.h
    batman.h
    civil.h
    geometria.h
    guardia.h
    niveles.h
    partida.h
    percepcion.h
    personaje.h
    razonamiento.h
    ventana.h
    vista.h
)

set(FUENTES
    accion.cpp
    aprendizaje.cpp
    audio.cpp
    batman.cpp
    civil.cpp
    guardia.cpp
    main.cpp
    niveles.cpp
    partida.cpp
    percepcion.cpp
    personaje.cpp
    razonamiento.cpp
    ventana.cpp
    vista.cpp
)

source_group("Header Files" FILES ${CABECERAS})
source_group("Source Files" FILES ${FUENTES})

add_executable(OperacionBronx WIN32
    ${CABECERAS}
    ${FUENTES}
    recursos/recursos.qrc
)
target_link_libraries(OperacionBronx PRIVATE Qt6::Widgets Qt6::Multimedia)
if(MSVC)
    target_compile_options(OperacionBronx PRIVATE /utf-8 /W4)
else()
    target_compile_options(OperacionBronx PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS OperacionBronx RUNTIME DESTINATION .)
