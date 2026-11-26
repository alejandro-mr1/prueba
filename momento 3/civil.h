#pragma once

#include "personaje.h"

class Civil final : public Personaje {
public:
    using Personaje::Personaje;
    void rescatar();
    void llegarASalvo();
    bool siguiendo() const { return siguiendo_; }
    bool aSalvo() const { return aSalvo_; }

private:
    bool siguiendo_ = false;
    bool aSalvo_ = false;
};
