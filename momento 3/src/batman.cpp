#include "batman.h"

#include <algorithm>

void Batman::actualizarHabilidades(double dt)
{
    detective_ = std::max(0.0, detective_ - dt);
    esperaDetective_ = std::max(0.0, esperaDetective_ - dt);
    esperaSalto_ = std::max(0.0, esperaSalto_ - dt);
}

bool Batman::activarDetective()
{
    if (esperaDetective_ > 0.0)
        return false;
    detective_ = 8.0;
    esperaDetective_ = 13.0;
    return true;
}

void Batman::cancelarDetective()
{
    detective_ = 0.0;
    esperaDetective_ = 0.0;
}
