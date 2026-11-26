#include "guardia.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace {
const std::array<Punto, 3> puntosZona{{{185, 355}, {483, 115}, {785, 307}}};
}

Guardia::Guardia(Punto inicio, std::vector<Punto> patrulla, bool aprende)
    : Personaje(inicio), inicio_(inicio), patrulla_(std::move(patrulla)), aprende_(aprende)
{
    if (patrulla_.empty())
        throw std::invalid_argument("Un guardia necesita al menos un punto de patrulla.");
    reiniciarPatrulla();
}

Punto Guardia::objetivoPatrulla()
{
    if (!desvio_ && puntoPatrulla_ == patrulla_.size() && aprende_) {
        if (const auto zona = aprendizaje_.zonaPrioritaria())
            desvio_ = puntosZona[*zona];
        else
            puntoPatrulla_ = 0;
    }
    return desvio_.value_or(patrulla_[puntoPatrulla_ % patrulla_.size()]);
}

void Guardia::avanzarPatrulla()
{
    if (desvio_) {
        desvio_.reset();
        puntoPatrulla_ = 0;
    } else {
        ++puntoPatrulla_;
        if (!aprende_ && puntoPatrulla_ == patrulla_.size())
            puntoPatrulla_ = 0;
    }
}

void Guardia::actualizar(Punto batman, double ruido, double dt, const PerfilDificultad& perfil,
                         const std::vector<Rectangulo>& paredes)
{
    const Punto desde = posicion();
    const bool ve = percepcion_.ver(desde, batman, angulo_, perfil, paredes);
    const bool oye = percepcion_.oir(desde, batman, ruido, perfil);
    if (ve || oye) {
        objetivo_ = batman;
        buscando_ = perfil.duracionBusqueda;
    } else {
        buscando_ = std::max(0.0, buscando_ - dt);
        if (buscando_ == 0.0) {
            objetivo_.reset();
            encuentroRegistrado_ = false;
        }
    }
    contacto_ = ve ? contacto_ + dt : std::max(0.0, contacto_ - 2.0 * dt);
    if (ve && aprende_ && !encuentroRegistrado_) {
        aprendizaje_.registrar(batman);
        encuentroRegistrado_ = true;
    }
    conducta_ = razonamiento_.decidir(ve, oye, objetivo_.has_value(), buscando_ > 0.0);
    Punto hacia = desde;
    double rapidez = perfil.rapidezPatrulla;
    if (conducta_ == Conducta::patrullar) {
        hacia = objetivoPatrulla();
        if (distancia(desde, hacia) < 10.0) {
            avanzarPatrulla();
            hacia = objetivoPatrulla();
        }
    } else if (objetivo_) {
        hacia = *objetivo_;
        rapidez = ve ? perfil.rapidezPersecucion : perfil.rapidezPatrulla * 1.14;
        if (!ve && !oye && distancia(desde, hacia) < 12.0)
            objetivo_.reset();
    } else {
        angulo_ += dt * 1.85;
    }
    const Punto siguiente = accion_.avanzar(desde, hacia, rapidez, dt, paredes);
    if (distancia(desde, siguiente) > 0.001)
        angulo_ = std::atan2(siguiente.y - desde.y, siguiente.x - desde.x);
    trasladar(siguiente, (siguiente - desde) * (1.0 / dt));
}

void Guardia::reiniciarPatrulla()
{
    trasladar(inicio_);
    puntoPatrulla_ = 0;
    objetivo_.reset();
    desvio_.reset();
    buscando_ = 0.0;
    contacto_ = 0.0;
    encuentroRegistrado_ = false;
    conducta_ = Conducta::patrullar;
    accion_.reiniciar();
    const Punto hacia = patrulla_.front() - inicio_;
    angulo_ = std::atan2(hacia.y, hacia.x);
}
