#include "aprendizaje.h"

#include <algorithm>

void Aprendizaje::registrar(Punto posicion)
{
    const int zona = std::clamp(static_cast<int>(posicion.x / (anchoVista / 3)), 0, 2);
    ++encuentros_[zona];
}

std::optional<std::size_t> Aprendizaje::zonaPrioritaria() const
{
    const auto mayor = std::max_element(encuentros_.begin(), encuentros_.end());
    if (*mayor == 0)
        return std::nullopt;
    return static_cast<std::size_t>(mayor - encuentros_.begin());
}
