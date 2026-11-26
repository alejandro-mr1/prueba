#pragma once

#include "geometria.h"

#include <array>
#include <cstddef>
#include <optional>

class Personaje {
public:
    explicit Personaje(Punto posicion) : posicion_(posicion) {}
    virtual ~Personaje() = default;
    Punto posicion() const { return posicion_; }
    Punto velocidad() const { return velocidad_; }
    int orientacion() const { return orientacion_; }
    void trasladar(Punto posicion, Punto velocidad = {});

private:
    Punto posicion_;
    Punto velocidad_;
    int orientacion_ = 1;
};

class Batman final : public Personaje {
public:
    using Personaje::Personaje;
    void actualizarHabilidades(double dt);
    bool activarDetective();
    void cancelarDetective();
    bool saltoEspecialDisponible() const { return esperaSalto_ <= 0.0; }
    void aterrizarTrasSaltoEspecial() { esperaSalto_ = 1.25; }
    double detectiveRestante() const { return detective_; }
    double esperaDetective() const { return esperaDetective_; }
    int evidencias() const { return evidencias_; }
    int detecciones() const { return detecciones_; }
    void recogerEvidencia() { ++evidencias_; }
    void registrarDeteccion() { ++detecciones_; }

private:
    double detective_ = 0.0;
    double esperaDetective_ = 0.0;
    double esperaSalto_ = 0.0;
    int evidencias_ = 0;
    int detecciones_ = 0;
};

class Civil final : public Personaje {
public:
    using Personaje::Personaje;
    void rescatar() { siguiendo_ = true; }
    void llegarASalvo() { siguiendo_ = false; aSalvo_ = true; }
    bool siguiendo() const { return siguiendo_; }
    bool aSalvo() const { return aSalvo_; }

private:
    bool siguiendo_ = false;
    bool aSalvo_ = false;
};

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
