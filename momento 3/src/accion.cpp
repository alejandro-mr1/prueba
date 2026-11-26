#include "accion.h"

#include <algorithm>
#include <array>
#include <limits>

namespace {
constexpr int celda = 16;
constexpr int columnas = static_cast<int>(anchoVista) / celda;
constexpr int filas = static_cast<int>(altoVista) / celda;
constexpr int totalCeldas = columnas * filas;
constexpr double radioGuardia = 9.0;

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
