#include "partida.h"

void Partida::iniciar(Dificultad dificultad)
{
    nivel_ = std::make_unique<Infiltracion>(dificultad);
    dificultad_ = dificultad;
    pantalla_ = Pantalla::jugando;
}

void Partida::continuarMision()
{
    if (pantalla_ == Pantalla::transicion) {
        nivel_ = std::make_unique<Rescate>();
        pantalla_ = Pantalla::jugando;
    }
}

void Partida::reiniciarNivel()
{
    if (rescate()) {
        nivel_ = std::make_unique<Rescate>();
        pantalla_ = Pantalla::jugando;
    } else if (infiltracion()) {
        iniciar(dificultad_);
    }
}

void Partida::cambiarPausa()
{
    if (pantalla_ == Pantalla::jugando)
        pantalla_ = Pantalla::pausa;
    else if (pantalla_ == Pantalla::pausa)
        pantalla_ = Pantalla::jugando;
}

void Partida::volverAlMenu()
{
    nivel_.reset();
    pantalla_ = Pantalla::menu;
}

void Partida::actualizar(const Entrada& entrada, double dt, std::vector<Evento>& eventos)
{
    if (pantalla_ != Pantalla::jugando)
        return;
    nivel_->actualizar(entrada, dt, eventos);
    switch (nivel_->resultado()) {
    case Resultado::victoria:
        pantalla_ = infiltracion() ? Pantalla::transicion : Pantalla::victoria;
        break;
    case Resultado::derrota:
        pantalla_ = Pantalla::derrota;
        break;
    case Resultado::enCurso:
        break;
    }
}

const Infiltracion* Partida::infiltracion() const { return dynamic_cast<const Infiltracion*>(nivel_.get()); }
const Rescate* Partida::rescate() const { return dynamic_cast<const Rescate*>(nivel_.get()); }
