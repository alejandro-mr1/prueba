#pragma once

#include "geometria.h"

class Personaje {
public:
    explicit Personaje(Punto posicion) : posicion_(posicion) {}
    virtual ~Personaje() = default;
    Punto posicion() const { return posicion_; }
    Punto velocidad() const { return velocidad_; }
    int orientacion() const { return orientacion_; }
    void trasladar(Punto posicion, Punto velocidad = {});

private:
    Punto posicion_;
    Punto velocidad_;
    int orientacion_ = 1;
};
