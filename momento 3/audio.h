#pragma once

#include "niveles.h"

#include <QSoundEffect>

class Audio {
public:
    Audio();
    void ambiente(bool activo);
    void reproducir(Evento evento);

private:
    QSoundEffect ambiente_;
    QSoundEffect evidencia_;
    QSoundEffect alerta_;
    QSoundEffect rescate_;
    QSoundEffect gancho_;
    bool ambienteActivo_ = false;
};
