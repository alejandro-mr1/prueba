#pragma once

#include "geometria.h"
#include "personaje.h"

#include <array>
#include <cstddef>
#include <optional>

struct PerfilDificultad {
    double alcanceVision;
    double semiApertura;
    double alcanceOido;
    double rapidezPatrulla;
    double rapidezPersecucion;
    double duracionBusqueda;
    double rapidezReflector;
    double exposicionReflector;
    double tiempoDeteccionVisual;
};

enum class Conducta { patrullar, investigar, perseguir, buscar };

class Percepcion {
public:
    bool ver(Punto origen, Punto objetivo, double angulo, const PerfilDificultad& perfil,
             const std::vector<Rectangulo>& paredes) const;
    bool oir(Punto origen, Punto ruido, double intensidad, const PerfilDificultad& perfil) const;
};

class Razonamiento {
public:
    Conducta decidir(bool ve, bool oye, bool hayObjetivo, bool quedaBusqueda) const;
};

class Aprendizaje {
public:
    void registrar(Punto posicion);
    std::optional<std::size_t> zonaPrioritaria() const;
    const std::array<int, 3>& encuentros() const { return encuentros_; }

private:
    std::array<int, 3> encuentros_{};
};

class Accion {
public:
    Punto avanzar(Punto desde, Punto hacia, double rapidez, double dt,
                  const std::vector<Rectangulo>& paredes);
    void reiniciar();

private:
    std::vector<Punto> ruta_;
    Punto destino_{};
    std::size_t siguiente_ = 0;
    double espera_ = 0.0;
};

class Guardia final : public Personaje {
public:
    Guardia(Punto inicio, std::vector<Punto> patrulla, bool aprende);
    void actualizar(Punto batman, double ruido, double dt, const PerfilDificultad& perfil,
                    const std::vector<Rectangulo>& paredes);
    void reiniciarPatrulla();
    Conducta conducta() const { return conducta_; }
    double angulo() const { return angulo_; }
    double contacto() const { return contacto_; }
    const Aprendizaje& aprendizaje() const { return aprendizaje_; }
    bool aprende() const { return aprende_; }

private:
    Punto objetivoPatrulla();
    void avanzarPatrulla();

    Punto inicio_;
    std::vector<Punto> patrulla_;
    bool aprende_;
    Percepcion percepcion_;
    Razonamiento razonamiento_;
    Accion accion_;
    Aprendizaje aprendizaje_;
    Conducta conducta_ = Conducta::patrullar;
    std::optional<Punto> objetivo_;
    std::optional<Punto> desvio_;
    std::size_t puntoPatrulla_ = 0;
    double angulo_ = 0.0;
    double buscando_ = 0.0;
    double contacto_ = 0.0;
    bool encuentroRegistrado_ = false;
};
