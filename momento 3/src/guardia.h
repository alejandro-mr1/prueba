#pragma once

#include "accion.h"
#include "aprendizaje.h"
#include "percepcion.h"
#include "personaje.h"
#include "razonamiento.h"

#include <cstddef>
#include <optional>
#include <vector>

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
