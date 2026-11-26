#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

inline constexpr double pi = 3.14159265358979323846;
inline constexpr double anchoVista = 960.0;
inline constexpr double altoVista = 560.0;

struct Punto {
    double x = 0.0;
    double y = 0.0;
};

inline Punto operator+(Punto a, Punto b) { return {a.x + b.x, a.y + b.y}; }
inline Punto operator-(Punto a, Punto b) { return {a.x - b.x, a.y - b.y}; }
inline Punto operator*(Punto p, double factor) { return {p.x * factor, p.y * factor}; }
inline double producto(Punto a, Punto b) { return a.x * b.x + a.y * b.y; }
inline double norma(Punto p) { return std::hypot(p.x, p.y); }
inline double distancia(Punto a, Punto b) { return norma(a - b); }
inline Punto direccion(double angulo) { return {std::cos(angulo), std::sin(angulo)}; }

inline Punto unitario(Punto p)
{
    const double longitud = norma(p);
    return longitud > 0.0001 ? p * (1.0 / longitud) : Punto{};
}

inline Punto acercar(Punto desde, Punto hacia, double paso)
{
    const Punto diferencia = hacia - desde;
    const double longitud = norma(diferencia);
    return longitud <= paso ? hacia : desde + diferencia * (paso / longitud);
}

struct Rectangulo {
    double x;
    double y;
    double ancho;
    double alto;

    double derecha() const { return x + ancho; }
    double abajo() const { return y + alto; }
    bool contiene(Punto p) const
    {
        return p.x >= x && p.x <= derecha() && p.y >= y && p.y <= abajo();
    }
    Rectangulo ampliado(double radio) const
    {
        return {x - radio, y - radio, ancho + 2 * radio, alto + 2 * radio};
    }
};

inline bool cortaSegmento(const Rectangulo& r, Punto inicio, Punto final)
{
    const Punto d = final - inicio;
    double entrada = 0.0;
    double salida = 1.0;
    const auto recortar = [&](double posicion, double delta, double minimo, double maximo) {
        if (std::abs(delta) < 0.0001)
            return posicion >= minimo && posicion <= maximo;
        const double a = (minimo - posicion) / delta;
        const double b = (maximo - posicion) / delta;
        entrada = std::max(entrada, std::min(a, b));
        salida = std::min(salida, std::max(a, b));
        return entrada <= salida;
    };
    return recortar(inicio.x, d.x, r.x, r.derecha()) &&
           recortar(inicio.y, d.y, r.y, r.abajo());
}

inline bool lineaLibre(Punto inicio, Punto final, const std::vector<Rectangulo>& paredes,
                       double margen = 0.0)
{
    return std::none_of(paredes.begin(), paredes.end(), [&](const Rectangulo& pared) {
        return cortaSegmento(pared.ampliado(margen), inicio, final);
    });
}

inline bool dentroDeCono(Punto origen, Punto objetivo, double angulo, double alcance, double apertura)
{
    const Punto delta = objetivo - origen;
    const double longitud = norma(delta);
    return longitud <= alcance && producto(delta, direccion(angulo)) >= longitud * std::cos(apertura);
}
