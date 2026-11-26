#include "personaje.h"

#include <cmath>

void Personaje::trasladar(Punto posicion, Punto velocidad)
{
    posicion_ = posicion;
    velocidad_ = velocidad;
    if (std::abs(velocidad.x) > 0.01)
        orientacion_ = velocidad.x < 0.0 ? -1 : 1;
}
