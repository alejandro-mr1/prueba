#include "razonamiento.h"

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
