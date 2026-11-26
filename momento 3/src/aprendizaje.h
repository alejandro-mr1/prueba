#pragma once

#include "geometria.h"

#include <array>
#include <cstddef>
#include <optional>

class Aprendizaje {
public:
    void registrar(Punto posicion);
    std::optional<std::size_t> zonaPrioritaria() const;
    const std::array<int, 3>& encuentros() const { return encuentros_; }

private:
    std::array<int, 3> encuentros_{};
};
