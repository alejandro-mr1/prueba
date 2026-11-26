#pragma once

#include "geometria.h"

#include <vector>

struct PerfilDificultad {
    double alcanceVision;
    double semiApertura;
    double alcanceOido;
    double rapidezPatrulla;
    double rapidezPersecucion;
    double duracionBusqueda;
    double rapidezReflector;
    double exposicionReflector;
    double tiempoDeteccionVisual;
};

class Percepcion {
public:
    bool ver(Punto origen, Punto objetivo, double angulo, const PerfilDificultad& perfil,
             const std::vector<Rectangulo>& paredes) const;
    bool oir(Punto origen, Punto ruido, double intensidad, const PerfilDificultad& perfil) const;
};
