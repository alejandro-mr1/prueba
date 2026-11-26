#pragma once

#include "geometria.h"

#include <cstddef>
#include <vector>

class Accion {
public:
    Punto avanzar(Punto desde, Punto hacia, double rapidez, double dt,
                  const std::vector<Rectangulo>& paredes);
    void reiniciar();

private:
    std::vector<Punto> ruta_;
    Punto destino_{};
    std::size_t siguiente_ = 0;
    double espera_ = 0.0;
};
