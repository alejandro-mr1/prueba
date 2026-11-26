#pragma once

#include "niveles.h"

enum class Pantalla { menu, jugando, pausa, transicion, derrota, victoria };

class Partida {
public:
    void iniciar(Dificultad dificultad);
    void continuarMision();
    void reiniciarNivel();
    void cambiarPausa();
    void volverAlMenu();
    void actualizar(const Entrada& entrada, double dt, std::vector<Evento>& eventos);

    const Infiltracion* infiltracion() const;
    const Rescate* rescate() const;
    Dificultad dificultad() const { return dificultad_; }
    Pantalla pantalla() const { return pantalla_; }

private:
    Dificultad dificultad_ = Dificultad::normal;
    Pantalla pantalla_ = Pantalla::menu;
    std::unique_ptr<Nivel> nivel_;
};
