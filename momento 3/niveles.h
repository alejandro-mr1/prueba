#pragma once

#include "batman.h"
#include "civil.h"
#include "guardia.h"
#include "percepcion.h"

#include <deque>
#include <memory>
#include <optional>
#include <vector>

struct Entrada {
    bool izquierda = false;
    bool derecha = false;
    bool arriba = false;
    bool abajo = false;
    bool sigilo = false;
    bool accion = false;
    bool detective = false;
    bool saltar = false;
    bool gancho = false;
};

enum class Evento { evidencia, alerta, rescate, gancho };
enum class Resultado { enCurso, victoria, derrota };

class Nivel {
public:
    virtual ~Nivel() = default;
    virtual void actualizar(const Entrada& entrada, double dt, std::vector<Evento>& eventos) = 0;
    const Batman& batman() const { return *batman_; }
    Resultado resultado() const { return resultado_; }

protected:
    explicit Nivel(Punto inicio) : batman_(std::make_unique<Batman>(inicio)) {}
    std::unique_ptr<Batman> batman_;
    Resultado resultado_ = Resultado::enCurso;
};

struct Reflector {
    Punto origen;
    double central;
    double amplitud;
    double frecuencia;
    double fase;
    double alcance;
    double semiApertura;

    double angulo(double tiempo, double factorRapidez) const;
};

struct Evidencia {
    Punto posicion;
    const char* nombre;
    bool recogida = false;
};

class Infiltracion final : public Nivel {
public:
    Infiltracion();
    void actualizar(const Entrada& entrada, double dt, std::vector<Evento>& eventos) override;

    const std::vector<Rectangulo>& paredes() const { return paredes_; }
    const std::vector<std::unique_ptr<Guardia>>& guardias() const { return guardias_; }
    const std::vector<Reflector>& reflectores() const { return reflectores_; }
    const std::vector<Evidencia>& evidencias() const { return evidencias_; }
    const PerfilDificultad& perfil() const { return perfil_; }
    const Rectangulo& salida() const { return salida_; }
    double tiempo() const { return tiempo_; }
    double alerta() const { return alerta_; }
    double proteccion() const { return proteccion_; }
    const char* aviso() const { return aviso_; }

private:
    bool posicionLibre(Punto posicion) const;
    void mover(const Entrada& entrada, double dt);
    void interactuar(std::vector<Evento>& eventos);
    void actualizarVigilancia(double ruido, double dt, std::vector<Evento>& eventos);
    void registrarDeteccion(std::vector<Evento>& eventos);

    static constexpr Punto inicio_{741, 484};
    Rectangulo salida_{469, 47, 76, 59};
    PerfilDificultad perfil_;
    std::vector<Rectangulo> paredes_;
    std::vector<std::unique_ptr<Guardia>> guardias_;
    std::vector<Reflector> reflectores_;
    std::vector<Evidencia> evidencias_;
    double tiempo_ = 0.0;
    double exposicion_ = 0.0;
    double alerta_ = 0.0;
    double proteccion_ = 0.0;
    double tiempoAviso_ = 0.0;
    const char* aviso_ = nullptr;
};

struct ParametrosRescate {
    double gravedad = 790.0;
    double rapidez = 238.0;
    double impulsoNormal = 318.0;
    double cuerdaMinima = 170.0;
    double cuerdaMaxima = 330.0;
    double rapidezGancho = 1.35;
};

class Pendulo {
public:
    Pendulo(Punto anclaje, Punto posicion, Punto velocidad, double rapidezMinima, int orientacion);
    void avanzar(double gravedad, double dt);
    Punto posicion() const;
    Punto velocidad() const;
    Punto anclaje() const { return anclaje_; }
    double longitud() const { return longitud_; }

private:
    Punto anclaje_;
    double longitud_;
    double angulo_;
    double rapidezAngular_;
};

class Rescate final : public Nivel {
public:
    explicit Rescate(ParametrosRescate parametros = {});
    void actualizar(const Entrada& entrada, double dt, std::vector<Evento>& eventos) override;

    const std::vector<Rectangulo>& techos() const { return techos_; }
    const std::vector<Punto>& anclajes() const { return anclajes_; }
    const std::vector<std::unique_ptr<Civil>>& civiles() const { return civiles_; }
    const std::optional<Pendulo>& pendulo() const { return pendulo_; }
    std::optional<Punto> anclajeDisponible() const;
    const Rectangulo& salida() const { return salida_; }
    double restante() const { return restante_; }
    double anchoMundo() const { return anchoMundo_; }
    bool enSuelo() const { return enSuelo_; }
    int aSalvo() const;

private:
    struct Muestra {
        double tiempo;
        Punto posicion;
    };

    void aplicarGancho(int direccion, std::vector<Evento>& eventos);
    void mover(const Entrada& entrada, int direccion, double dt);
    void moverPendulo(double dt);
    bool aterrizar(Punto anterior, Punto& siguiente, Punto& velocidad, const Rectangulo& solido);
    void rescatarCercano(std::vector<Evento>& eventos);
    void actualizarCiviles(double dt);
    void reaparecer();

    ParametrosRescate parametros_;
    std::vector<Rectangulo> techos_;
    std::vector<Punto> anclajes_;
    std::vector<std::unique_ptr<Civil>> civiles_;
    std::optional<Pendulo> pendulo_;
    std::deque<Muestra> recorrido_;
    Rectangulo salida_{2590, 328, 130, 76};
    Punto control_{110, 410};
    double restante_ = 180.0;
    double preparacionGancho_ = 0.0;
    int direccionGancho_ = 1;
    double anchoMundo_ = 2740.0;
    bool enSuelo_ = true;
};
