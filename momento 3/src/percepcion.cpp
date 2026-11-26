#include "percepcion.h"

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
