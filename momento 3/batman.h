#pragma once

#include "personaje.h"

class Batman final : public Personaje {
public:
    using Personaje::Personaje;
    void actualizarHabilidades(double dt);
    bool activarDetective();
    void cancelarDetective();
    double detectiveRestante() const { return detective_; }
    double esperaDetective() const { return esperaDetective_; }
    int evidencias() const { return evidencias_; }
    int detecciones() const { return detecciones_; }
    void recogerEvidencia() { ++evidencias_; }
    void registrarDeteccion() { ++detecciones_; }

private:
    double detective_ = 0.0;
    double esperaDetective_ = 0.0;
    int evidencias_ = 0;
    int detecciones_ = 0;
};
