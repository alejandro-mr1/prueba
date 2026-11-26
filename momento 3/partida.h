#pragma once

#include "niveles.h"

enum class Pantalla { menu, jugando, transicion, derrota, victoria };

class Partida {
public:
    void iniciar();
    void continuarMision();
    void reiniciarNivel();
    void volverAlMenu();
    void actualizar(const Entrada& entrada, double dt, std::vector<Evento>& eventos);

    const Infiltracion* infiltracion() const;
    const Rescate* rescate() const;
    Pantalla pantalla() const { return pantalla_; }

private:
    Pantalla pantalla_ = Pantalla::menu;
    std::unique_ptr<Nivel> nivel_;
};
