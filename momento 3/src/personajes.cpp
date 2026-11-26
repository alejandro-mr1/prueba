#include "personajes.h"

#include <array>
#include <limits>
#include <stdexcept>
#include <utility>

namespace {
constexpr int celda = 16;
constexpr int columnas = static_cast<int>(anchoVista) / celda;
constexpr int filas = static_cast<int>(altoVista) / celda;
constexpr int totalCeldas = columnas * filas;
constexpr double radioGuardia = 9.0;
const std::array<Punto, 3> puntosZona{{{185, 355}, {483, 115}, {785, 307}}};

Punto centro(int indice)
{
    return {static_cast<double>(indice % columnas * celda + celda / 2),
            static_cast<double>(indice / columnas * celda + celda / 2)};
}

int celdaCercana(Punto p, const std::array<bool, totalCeldas>& bloqueada)
{
    double menor = std::numeric_limits<double>::infinity();
    int resultado = -1;
    for (int i = 0; i < totalCeldas; ++i) {
        if (bloqueada[i])
            continue;
        const Punto delta = centro(i) - p;
        const double distanciaCuadrada = producto(delta, delta);
        if (distanciaCuadrada < menor) {
            menor = distanciaCuadrada;
            resultado = i;
        }
    }
    return resultado;
}

std::vector<Punto> buscarRuta(Punto origen, Punto destino, const std::vector<Rectangulo>& paredes)
{
    std::array<bool, totalCeldas> bloqueada{};
    for (int i = 0; i < totalCeldas; ++i) {
        bloqueada[i] = std::any_of(paredes.begin(), paredes.end(), [&](const Rectangulo& pared) {
            return pared.ampliado(radioGuardia).contiene(centro(i));
        });
    }
    const int inicio = celdaCercana(origen, bloqueada);
    const int final = celdaCercana(destino, bloqueada);
    if (inicio < 0 || final < 0)
        return {};

    std::array<int, totalCeldas> previo;
    previo.fill(-1);
    std::array<int, totalCeldas> cola{};
    int cabeza = 0;
    int fin = 0;
    previo[inicio] = inicio;
    cola[fin++] = inicio;
    while (cabeza < fin && previo[final] == -1) {
        const int actual = cola[cabeza++];
        const int x = actual % columnas;
        const int y = actual / columnas;
        const std::array<int, 4> vecinos{{x > 0 ? actual - 1 : -1,
            x + 1 < columnas ? actual + 1 : -1, y > 0 ? actual - columnas : -1,
            y + 1 < filas ? actual + columnas : -1}};
        for (const int vecino : vecinos) {
            if (vecino < 0 || bloqueada[vecino] || previo[vecino] != -1 ||
                !lineaLibre(centro(actual), centro(vecino), paredes, radioGuardia))
                continue;
            previo[vecino] = actual;
            cola[fin++] = vecino;
        }
    }
    if (previo[final] == -1)
        return {};

    std::vector<Punto> ruta;
    for (int actual = final; actual != inicio; actual = previo[actual])
        ruta.push_back(centro(actual));
    ruta.push_back(centro(inicio));
    std::reverse(ruta.begin(), ruta.end());
    return ruta;
}
}

void Personaje::trasladar(Punto posicion, Punto velocidad)
{
    posicion_ = posicion;
    velocidad_ = velocidad;
    if (std::abs(velocidad.x) > 0.01)
        orientacion_ = velocidad.x < 0.0 ? -1 : 1;
}

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

bool Percepcion::ver(Punto origen, Punto objetivo, double angulo, const PerfilDificultad& perfil,
                     const std::vector<Rectangulo>& paredes) const
{
    return dentroDeCono(origen, objetivo, angulo, perfil.alcanceVision, perfil.semiApertura) &&
           lineaLibre(origen, objetivo, paredes);
}

bool Percepcion::oir(Punto origen, Punto ruido, double intensidad, const PerfilDificultad& perfil) const
{
    return intensidad > 0.0 && distancia(origen, ruido) < intensidad * perfil.alcanceOido;
}

Conducta Razonamiento::decidir(bool ve, bool oye, bool hayObjetivo, bool quedaBusqueda) const
{
    if (ve)
        return Conducta::perseguir;
    if (oye)
        return Conducta::investigar;
    if (hayObjetivo || quedaBusqueda)
        return Conducta::buscar;
    return Conducta::patrullar;
}

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

Punto Accion::avanzar(Punto desde, Punto hacia, double rapidez, double dt,
                      const std::vector<Rectangulo>& paredes)
{
    espera_ -= dt;
    if (lineaLibre(desde, hacia, paredes, radioGuardia)) {
        reiniciar();
        return acercar(desde, hacia, rapidez * dt);
    }
    if (espera_ <= 0.0 || distancia(destino_, hacia) > 36.0) {
        ruta_ = buscarRuta(desde, hacia, paredes);
        destino_ = hacia;
        siguiente_ = 0;
        espera_ = 0.45;
    }
    if (siguiente_ >= ruta_.size())
        return desde;
    while (siguiente_ + 1 < ruta_.size() &&
           lineaLibre(desde, ruta_[siguiente_ + 1], paredes, radioGuardia))
        ++siguiente_;
    return acercar(desde, ruta_[siguiente_], rapidez * dt);
}

void Accion::reiniciar()
{
    ruta_.clear();
    siguiente_ = 0;
    espera_ = 0.0;
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
